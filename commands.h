#include <stdint.h>
#include "types.h"
extern const char *CmdAddHandler(bloom_cell *bloom, const uint64_t hashes[]);
extern const char *CmdCheckHandler(bloom_cell *bloom, const uint64_t hashes[]);
extern const char *CmdCheckThenAddHandler(bloom_cell *bloom, const uint64_t hashes[]);
extern void* HandlerTable[3][2];
