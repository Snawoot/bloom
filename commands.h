#include <stdint.h>
#include "types.h"
extern const char *CmdAddHandler(bloom_cell *, const char []);
extern const char *CmdCheckHandler(bloom_cell *, const char []);
extern const char *CmdCheckThenAddHandler(bloom_cell *, const char []);
extern void* HandlerTable[3][2];
