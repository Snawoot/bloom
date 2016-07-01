#include <stdio.h>
#include <stdlib.h>

const char usage_tpl[] = "\
Usage: %s [options] SNAPSHOT_FILE\n\
\n\
Options:\n\n\
-H BIND_ADDRESS		HTTP interface bind address. Default: 0.0.0.0\n\n\
-P BIND_PORT		HTTP interface bind port. Default: 8889\n\n\
-h			Print this help message\n\n\
-m M			Number of bits in bloom filter. Default: 2^33\n\n\
-k K			Number of hash functions. Default: 10\n\n\
-t SECONDS		Dump bloom filter snapshot to file every SECONDS\n\
			seconds. You can set this value to 0 if you wish\n\
			to disable this feature - snapshots are taken on USR1\n\
			signal and at exit in any case.\n";

void crash(char *msg, int code)
{
    fputs(msg, stderr);
    exit(code);
}

void usage(char *progname) {
    fprintf(stderr, usage_tpl, progname);
    exit(1);
}

