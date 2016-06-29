#include <stdint.h>
#include <stdbool.h>

#include "defines.h"
#include "types.h"
#include "ops.h"

//URI (commands) handlers
#define miss_response  "MISSING\n"
#define hit_response   "PRESENT\n"
#define added_response "ADDED\n"
const char *CmdAddHandler(bloom_cell *bloom, const uint64_t hashes[])
{
    int i;
    for (i=0; i<k; i++)
        JumpBit(bloom, hashes[i]);
    return added_response;
}

const char *CmdCheckHandler(bloom_cell *bloom, const uint64_t hashes[])
{
    int i;
    for (i=0; i<k; i++)
        if (!GetBit(bloom, hashes[i]))
            return miss_response;
    return hit_response;
}

const char *CmdCheckThenAddHandler(bloom_cell *bloom, const uint64_t hashes[])
{
    bool present = true;
    int i;
    for (i=0; i<k; i++)
        if (!GetBit(bloom, hashes[i])) {
            present = false;
            break;
        }
    if (!present)
        for (i=0; i<k; i++)
            JumpBit(bloom, hashes[i]);
    return present ? hit_response : miss_response;
}

//Request routing
void* HandlerTable[][2] = {
{"/add",            CmdAddHandler           },
{"/check",          CmdCheckHandler         },
{"/checkthenadd",   CmdCheckThenAddHandler  },
};

