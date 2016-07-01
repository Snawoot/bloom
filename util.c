#include <stdio.h>
#include <stdlib.h>

const char usage_tpl[] = "\
Usage: %s [options] SNAPSHOT_FILE\n\
\n\
Options:\n\
-H BIND_ADDRESS		HTTP interface bind address. Default: 0.0.0.0\n\
-P BIND_PORT		HTTP interface bind port. Default: 8889\n\
-h			Print this help message\n\
-m M			Number of bits in bloom filter. Default: 2^33\n\
-k K			Number of hash functions. Default: 10\n";

void crash(char *msg, int code)
{
    fputs(msg, stderr);
    exit(code);
}

void usage(char *progname) {
    fprintf(stderr, usage_tpl, progname);
    exit(1);
}

