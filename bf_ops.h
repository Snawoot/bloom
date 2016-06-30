#include <stdint.h>
#include <stdbool.h>
#include "bf_types.h"
bool bf_test_bit(bloom_cell *, uint64_t);
void bf_jump_bit(bloom_cell *, uint64_t);
void bf_add(bloom_cell *, const char []);
bool bf_check(bloom_cell *, const char []);
bool bf_check_then_add(bloom_cell *, const char []);
