//Server globals
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "types.h"
extern struct event_base *base;
extern struct event *dump_event;
extern struct evhttp *http;
extern struct evhttp_bound_socket *handle;
extern bloom_cell *Bloom;
extern char *snap_path;
extern bool dumper_active;
extern pid_t dumper;

