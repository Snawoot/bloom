#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf_types.h"

const char inprogress_sf[] = ".inprogress";
//Storage operations
int bf_dump_to_file(const bloom_filter_t *bf, const char *fname)
{
    fputs("Saving snapshot...\n", stderr);

    char *inprogress_fn = malloc(strlen(fname) + sizeof(inprogress_sf));
    strcpy(inprogress_fn, fname);
    strcat(inprogress_fn, inprogress_sf);

    size_t shouldwrite = (bf->m + (CHAR_BIT - 1)) / CHAR_BIT;
    FILE *f = fopen(inprogress_fn, "wb");

    if (f) {
        size_t bwritten = fwrite(bf->space, 1, shouldwrite, f);
        if (bwritten != shouldwrite) {
            fprintf(stderr, "Should write: %ld bytes. Written %ld bytes.\n", shouldwrite, bwritten);
            free(inprogress_fn);
            return -1;
        }
        fclose(f);
        if (rename(inprogress_fn, fname) == 0) {
            free(inprogress_fn);
            return 0;
        } else {
            free(inprogress_fn);
            fputs("Unable to rename inprogress snapshot into actual location.\n", stderr);
            return -1;
        }
    } else {
        fputs("Error opening file for writting.\n", stderr);
        free(inprogress_fn);
        return -1;
    }
}

int bf_load_from_file(bloom_filter_t *bf, const char *fname)
{
    FILE *f = fopen(fname, "rb");
    if (f) {
        size_t shouldread = (bf->m + (CHAR_BIT - 1)) / CHAR_BIT;
        size_t bread = fread(bf->space, 1, shouldread, f);
        if (bread != shouldread) {
            fprintf(stderr, "Should read: %ld bytes. Read %ld bytes.\n", shouldread, bread);
            return -1;
        }
        fclose(f);
        return 0;
    } else {
        fputs("Error opening file for reading.\n", stderr);
        return -1;
    }
}
