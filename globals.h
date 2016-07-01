//Server globals
#include <stdbool.h>
#include <unistd.h>
#include <event.h>
#include "bf_types.h"

extern struct event_base *base;
extern bloom_filter_t *Bloom;
extern char *snap_path;
extern bool dumper_active;
extern pid_t dumper;
