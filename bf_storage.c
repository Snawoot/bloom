#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "bf_types.h"

#define BYTES_FOR_PID sizeof('.') + sizeof('-') + 2.5 * sizeof(pid_t) + sizeof('\0') + 1
/* bytes for decimal record of int := 
:= ceil(log10(INT_MAX)) + sign + zero-byte (+ 1 to avoid real ceil call)
*/

//Storage operations
int _bf_commit_snapshot(const char *src, const char *dst) {
    int ret = -1;
    if (rename(src, dst) == 0) {
        ret = 0;
    } else {
        fputs("Unable to rename inprogress snapshot into actual location.\n",
            stderr);
    }
    return ret;
}

int bf_dump_to_file(const bloom_filter_t *bf, const char *fname)
{
    int ret = -1;
    fputs("Saving snapshot...\n", stderr);

    char *inprogress_fn = malloc(strlen(fname) + BYTES_FOR_PID);
    sprintf(inprogress_fn, "%s.%d", fname, getpid());

    size_t shouldwrite = (bf->m + (CHAR_BIT - 1)) / CHAR_BIT;
    FILE *f = fopen(inprogress_fn, "wb");

    if (f) {
        size_t bwritten = fwrite(bf->space, 1, shouldwrite, f);
        if (bwritten == shouldwrite) {
            fclose(f);
            ret = _bf_commit_snapshot(inprogress_fn, fname);
        } else {
            fprintf(stderr, "Should write: %ld bytes. Written %ld bytes.\n",
                shouldwrite, bwritten);
        }
    } else {
        fputs("Error opening file for writting.\n", stderr);
    }
    free(inprogress_fn);
    return ret;
}

int bf_load_from_file(bloom_filter_t *bf, const char *fname)
{
    int ret = -1;
    FILE *f = fopen(fname, "rb");
    if (f) {
        size_t shouldread = (bf->m + (CHAR_BIT - 1)) / CHAR_BIT;
        size_t bread = fread(bf->space, 1, shouldread, f);
        if (bread == shouldread) {
            fclose(f);
            ret = 0;
        } else {
            fprintf(stderr, "Should read: %ld bytes. Read %ld bytes.\n",
                shouldread, bread);
        }
    } else {
        fputs("Error opening file for reading.\n", stderr);
    }
    return ret;
}
