#include "types.h"
#include "ops.h"

//URI (commands) handlers
#define miss_response  "MISSING\n"
#define hit_response   "PRESENT\n"
#define added_response "ADDED\n"
const char *CmdAddHandler(bloom_cell *bloom, const char element[]) {
    bf_add(bloom, element);
    return added_response;
}

const char *CmdCheckHandler(bloom_cell *bloom, const char element[]) {
    return bf_check(bloom, element) ? hit_response : miss_response;
}

const char *CmdCheckThenAddHandler(bloom_cell *bloom, const char element[]) {
    return bf_check_then_add(bloom, element) ?
        hit_response : miss_response;
}

//Request routing
void* HandlerTable[][2] = {
{"/add",            CmdAddHandler           },
{"/check",          CmdCheckHandler         },
{"/checkthenadd",   CmdCheckThenAddHandler  },
};

