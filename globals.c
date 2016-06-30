#include <stdbool.h>
#include <evhttp.h>
#include <unistd.h>
#include "bf_types.h"

//Server globals
struct event_base *base = NULL;
struct event *dump_event = NULL;
struct evhttp *http = NULL;
struct evhttp_bound_socket *handle = NULL;
bloom_cell *Bloom = NULL;
char *snap_path = NULL;
bool dumper_active = false;
pid_t dumper;
