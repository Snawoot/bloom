#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <evhttp.h>
#include <unistd.h>
#include "globals.h"
#include "bf_storage.h"

void term_handler(int signo)
{
    event_base_loopexit(base, NULL);
}

void dump_handler(evutil_socket_t fd, short which, void *arg)
{
    if (dumper_active)
        return;
    dumper_active = true;
    pid_t pid = fork();
    if (pid == 0) {
        if (snap_path && Bloom) {
            bf_dump_to_file(Bloom, snap_path);
            exit(0);
        }
        else {
            fputs("Global pointers not set!\n", stderr);
            exit(11);
        }
    } else {
        if (pid == -1) {
            fputs("Unable to fork!\n", stderr);
            dumper_active = false;
        } else
        {
            dumper = pid;
        }
    }
}

void child_collector(int signo)
{
    int status;
    pid_t pid = waitpid(dumper, &status, WNOHANG);
    if (pid == dumper) {
        dumper_active = false;
        fprintf(stderr, "Dumper process %d exited with code %d. Collected him.\n", pid, WEXITSTATUS(status));
    }
}
