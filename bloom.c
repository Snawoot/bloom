#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

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
    bf_m_t m = 1 << 33;
    bf_k_t k = 10;
    int intconv;
    char *cnv;
    int c;

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
        case '?':
            usage(argv[0]);
            break;
        }

    if (optind != argc - 1)
        usage(argv[0]);
    snap_path = argv[optind];

    //Allocate memory
    fprintf(stderr, "Creating space ...\n");
    Bloom = bf_create(m, k);
    if (!Bloom)
        crash("Couldn`t initialize bloom filter with given parameters\n", -1);


    //Load or create snapshot file
    if (!access(snap_path, F_OK)) {
        fputs("Loading snapshot...\n", stderr);
        if (bf_load_from_file(Bloom, snap_path)) {
            fputs("Unable to load snapshot!\n", stderr);
            return -1;
        }
        fputs("Snapshot loaded.\n", stderr);
    } else {
        fputs("Initializing new file storage...\n", stderr);

        if (bf_dump_to_file(Bloom, snap_path)) {
            fputs("Unable to save initial snapshot!\n", stderr);
            return -1;
        }
        fputs("Initial snapshot written.\n", stderr);
    }

    if (!(base = event_base_new()))
        crash("Couldn't create an event_base: exiting\n", -1);

    if (!(http = evhttp_new(base)))
        crash("Couldn't create evhttp. Exiting.\n", -1);
    evhttp_set_gencb(http, OnReq, NULL);

    if (!(handle = evhttp_bind_socket_with_handle(http, bind_address, bind_port)))
        crash("couldn't bind to port. Exiting.\n", -1);

    if (signal(SIGINT, term_handler) == SIG_ERR)
        crash("Unable to set SIGINT handler!", -1);

    if (signal(SIGTERM, term_handler) == SIG_ERR)
        crash("Unable to set SIGTERM handler!", -1);

    if (signal(SIGCHLD, child_collector) == SIG_ERR)
        crash("Unable to set SIGCHLD handler!", -1);
    
    //This signal handled by event loop in order to avoid malloc deadlock
    if ((dump_event = evsignal_new(base, SIGUSR1, dump_handler, NULL)) == NULL)
        crash("Unable to create SIGUSR1 handler!", -1);
    else 
        if (event_add(dump_event, NULL) == -1)
            crash("Unable to add SIGUSR1 handler!", -1);

    if (event_base_dispatch(base) == -1)
        crash("Failed to run message loop.\n", -1);

    fputs("Exiting...\n", stderr);
    bf_dump_to_file(Bloom, snap_path);
    evhttp_del_accept_socket(http, handle);
    evhttp_free(http);
    event_free(dump_event);
    event_base_free(base);
    bf_destroy(Bloom);
    return 0;
}
