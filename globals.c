#include <stdbool.h>
#include <evhttp.h>
#include <unistd.h>
#include "bf_types.h"
#include <event.h>

//Server globals
struct event_base *base = NULL;
bloom_filter_t *Bloom = NULL;
char *snap_path = NULL;
bool dumper_active = false;
pid_t dumper;
