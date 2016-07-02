#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <event.h>
#include <evhttp.h>

#include "sighandlers.h"
#include "bf_ops.h"
#include "bf_storage.h"
#include "bf_types.h"
#include "util.h"
#include "handler.h"

#include "globals.c"

//Main
int main(int argc, char *argv[])
{
    signal(SIGUSR1, SIG_IGN); //Suppress dumper signals during startup

    uint16_t bind_port = 8889;
    char *bind_address = "0.0.0.0";
    bf_m_t m = 8589934592;
    bf_k_t k = 10;
    int intconv;
    char *cnv;
    int c;

    struct event_base *base = NULL;
    struct event *dump_event, *int_event, *term_event, *timer_event, *child_event;
    struct evhttp *http = NULL;
    struct evhttp_bound_socket *handle = NULL;

    struct timeval timer_tv;
    timer_tv.tv_sec = 300;
    timer_tv.tv_usec = 0;

    //Get args
    opterr = 0;

    while ((c = getopt (argc, argv, "hH:P:m:k:t:")) != -1)
        switch (c) {
        case 'H':
            bind_address = optarg;
            break;
        case 'P':
            intconv = atoi(optarg);
            if (intconv <= 0 || intconv > UINT16_MAX) usage(argv[0]);
                else bind_port = intconv;
            break;
        case 'h':
            usage(argv[0]);
            break;
        case 'm':
            m = strtoull(optarg, &cnv, 10);
            if (!m || *cnv) usage(argv[0]);
            break;
        case 'k':
            intconv = atoi(optarg);
            if (intconv <= 0 || intconv > UINT8_MAX) usage(argv[0]);
                else k = intconv;
            break;
        case 't':
            timer_tv.tv_sec = strtol(optarg, &cnv, 10);
            if (timer_tv.tv_sec < 0 || *cnv) usage(argv[0]);
            break;
        case '?':
            usage(argv[0]);
            break;
        }

    if (optind != argc - 1)
        usage(argv[0]);
    snap_path = argv[optind];

    fprintf(stderr, "Creating space ...\n");

    //Load or create snapshot file
    if (!access(snap_path, F_OK)) {
        fputs("Loading snapshot...\n", stderr);
        if (!(Bloom = bf_load_from_file(snap_path))) {
            crash("Unable to load snapshot!\n", -1);
        }
        fputs("Snapshot loaded.\n", stderr);
    } else {
        fputs("Initializing new file storage...\n", stderr);

        if (!(Bloom = bf_create(m, k)))
            crash("Couldn`t initialize bloom filter with given parameters\n", -1);
        if (bf_dump_to_file(Bloom, snap_path))
            crash("Unable to save initial snapshot!\n", -1);
        fputs("Initial snapshot written.\n", stderr);
    }

    if (!(base = event_base_new()))
        crash("Couldn't create an event_base: exiting\n", -1);

    if (!(http = evhttp_new(base)))
        crash("Couldn't create evhttp. Exiting.\n", -1);
    evhttp_set_gencb(http, OnReq, NULL);

    if (!(handle = evhttp_bind_socket_with_handle(http, bind_address, bind_port)))
        crash("couldn't bind to port. Exiting.\n", -1);

    if ((term_event = evsignal_new(base, SIGTERM, term_handler, base)) == NULL)
        crash("Unable to create SIGTERM handler!", -1);
    else 
        if (event_add(term_event, NULL) == -1)
            crash("Unable to add SIGTERM handler!", -1);

    if ((int_event = evsignal_new(base, SIGINT, term_handler, base)) == NULL)
        crash("Unable to create SIGINT handler!", -1);
    else 
        if (event_add(int_event, NULL) == -1)
            crash("Unable to add SIGINT handler!", -1);

    if ((child_event = evsignal_new(base, SIGCHLD, child_collector, NULL)) == NULL)
        crash("Unable to create SIGCHLD handler!", -1);
    else 
        if (event_add(child_event, NULL) == -1)
            crash("Unable to add SIGCHLD handler!", -1);

    //This signal handled by event loop in order to avoid malloc deadlock
    if ((dump_event = evsignal_new(base, SIGUSR1, dump_handler, NULL)) == NULL)
        crash("Unable to create SIGUSR1 handler!", -1);
    else 
        if (event_add(dump_event, NULL) == -1)
            crash("Unable to add SIGUSR1 handler!", -1);

    if (timer_tv.tv_sec) {
        if ((timer_event = event_new(base, -1, EV_PERSIST, dump_handler, NULL)) == NULL)
            crash("Unable to create timer handler!", -1);
        else
            if (evtimer_add(timer_event, &timer_tv) == -1)
                crash("Unable to add timer handler!", -1);
    }

    if (event_base_dispatch(base) == -1)
        crash("Failed to run message loop.\n", -1);

    fputs("Exiting...\n", stderr);
    bf_dump_to_file(Bloom, snap_path);
    evhttp_del_accept_socket(http, handle);
    evhttp_free(http);
    event_free(dump_event);
    event_free(term_event);
    event_free(int_event);
    event_free(timer_event);
    event_free(child_event);
    event_base_free(base);
    bf_destroy(Bloom);
    return 0;
}
