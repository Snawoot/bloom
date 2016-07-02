#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <evhttp.h>
#include <unistd.h>
#include "globals.h"
#include "bf_storage.h"

#define BYTES_FOR_PID sizeof('.') + sizeof('-') + 2.5 * sizeof(pid_t) + sizeof('\0') + 1
/* bytes for decimal record of int := 
:= ceil(log10(INT_MAX)) + sign + zero-byte (+ 1 to avoid real ceil call)
*/

bool dumper_active = false;
pid_t dumper;

void term_handler(evutil_socket_t fd, short which, void *base)
{
    if (dumper_active) {
        kill(dumper, SIGKILL); // REDIS STYLE NOW!!!

        char *inprogress_fn = malloc(strlen(snap_path) + BYTES_FOR_PID);
        sprintf(inprogress_fn, "%s.%d", snap_path, dumper);
        unlink(inprogress_fn);
    }

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

void child_collector(evutil_socket_t fd, short which, void *arg)
{
    int status;
    pid_t pid = waitpid(dumper, &status, WNOHANG);
    if (pid == dumper) {
        dumper_active = false;
        fprintf(stderr, "Dumper process %d exited with code %d. Collected him.\n", pid, WEXITSTATUS(status));
    }
}
