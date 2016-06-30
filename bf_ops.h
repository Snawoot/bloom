#include <stdint.h>
#include <stdbool.h>
#include "bf_types.h"
bool _bf_test_bit(bf_cell_t *, uint64_t);
void _bf_jump_bit(bf_cell_t *, uint64_t);
void bf_add(bloom_filter_t *, const char []);
bool bf_check(bloom_filter_t *, const char []);
bool bf_check_then_add(bloom_filter_t *, const char []);
bloom_filter_t *bf_create(bf_m_t, bf_k_t);
void bf_destroy(bloom_filter_t *bf);
