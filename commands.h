#include <stdint.h>
#include "bf_types.h"
extern const char *CmdAddHandler(bloom_filter_t *, const char []);
extern const char *CmdCheckHandler(bloom_filter_t *, const char []);
extern const char *CmdCheckThenAddHandler(bloom_filter_t *, const char []);
extern void* HandlerTable[3][2];
