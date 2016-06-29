#include <stdio.h>
#include "types.h"
#include "defines.h"

//Storage operations
int SaveSnap(const bloom_cell *bloom, const char *fname)
{
    fputs("Saving snapshot...\n", stderr);
    size_t shouldwrite = (m + (CHAR_BIT - 1)) / CHAR_BIT;
    FILE *f = fopen(fname, "wb");
    if (f) {
        size_t bwritten = fwrite(bloom, 1, shouldwrite, f);
        if (bwritten != shouldwrite) {
            fprintf(stderr, "Should write: %ld bytes. Written %ld bytes.\n", shouldwrite, bwritten);
            return -1;
        }
        fclose(f);
        return 0;
    } else {
        fputs("Error opening file for writting.\n", stderr);
        return -1;
    }
}

int LoadSnap(bloom_cell *bloom, const char *fname)
{
    FILE *f = fopen(fname, "rb");
    if (f) {
        size_t shouldread = (m + (CHAR_BIT - 1)) / CHAR_BIT;
        size_t bread = fread(bloom, 1, shouldread, f);
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
