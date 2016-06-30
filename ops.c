#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "defines.h"

//Bloom operations
bool GetBit(bloom_cell *bv, uint64_t n)
{
    return (bv[n / BITS_PER_CELL] & ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL)))) != 0;
}

void JumpBit(bloom_cell *bv, uint64_t n)
{
    bv[n / BITS_PER_CELL] |= ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL )) );
}

