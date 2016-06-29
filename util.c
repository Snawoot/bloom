#include <stdio.h>
#include <stdlib.h>

void crash(char *msg, int code)
{
    fputs(msg, stderr);
    exit(code);
}

