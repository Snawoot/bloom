#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "defines.h"
#include "hash.h"

//Bloom operations
bool bf_test_bit(bloom_cell *bv, uint64_t n) {
    return (bv[n / BITS_PER_CELL] & ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL)))) != 0;
}

void bf_jump_bit(bloom_cell *bv, uint64_t n) {
    bv[n / BITS_PER_CELL] |= ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL )) );
}

void bf_add(bloom_cell *bloom, const char element[]) {
    uint64_t Ki[k];
    bf_hashes(element, Ki);
    int i;
    for (i=0; i<k; i++)
        bf_jump_bit(bloom, Ki[i]);
}

bool bf_check(bloom_cell *bloom, const char element[]) {
    //calc hash here
    uint64_t Ki[k];
    bf_hashes(element, Ki);
    int i;
    for (i=0; i<k; i++)
        if (!bf_test_bit(bloom, Ki[i]))
            return false;
    return true;
}

bool bf_check_then_add(bloom_cell *bloom, const char element[]) {
    //calc hash here
    uint64_t Ki[k];
    bf_hashes(element, Ki);

    bool present = true;
    int i;
    for (i=0; i<k; i++)
        if (!bf_test_bit(bloom, Ki[i])) {
            present = false;
            break;
        }
    if (!present)
        for (i=0; i<k; i++)
            bf_jump_bit(bloom, Ki[i]);
    return present;
}
