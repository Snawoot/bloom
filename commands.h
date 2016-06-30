#include <stdint.h>
#include "bf_types.h"
extern const char *CmdAddHandler(bloom_cell *, const char []);
extern const char *CmdCheckHandler(bloom_cell *, const char []);
extern const char *CmdCheckThenAddHandler(bloom_cell *, const char []);
extern void* HandlerTable[3][2];
