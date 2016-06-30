#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bf_types.h"
#include "bf_hash.h"

//Bloom operations
bool _bf_test_bit(bf_cell_t *bv, bf_m_t n) {
    return (bv[n / BF_BITS_PER_CELL] & ((bf_cell_t)1 << ((BF_BITS_PER_CELL - 1) - (n % BF_BITS_PER_CELL)))) != 0;
}

void _bf_jump_bit(bf_cell_t *bv, bf_m_t n) {
    bv[n / BF_BITS_PER_CELL] |= ((bf_cell_t)1 << ((BF_BITS_PER_CELL - 1) - (n % BF_BITS_PER_CELL )) );
}

bf_m_t _bf_make_m_power_of_two(bf_m_t m, bf_m_t *new_m, bf_hp_t *power) {
    bf_hp_t hp = 1;
    bf_m_t aligned_m = 2;
    do {
        hp++;
        aligned_m *= 2;
    } while (aligned_m < m && hp < ( sizeof(m) * 8));

    if (new_m) *new_m = aligned_m;
    if (power) *power = hp;
    return aligned_m;
}

void bf_add(bloom_filter_t *bf, const char element[]) {
    bf_k_t k = bf->k;
    bf_m_t Ki[k];
    bf_hashes(element, Ki, k, bf->hash_part);
    bf_k_t i;
    for (i=0; i<k; i++)
        _bf_jump_bit(bf->space, Ki[i]);
}

bool bf_check(bloom_filter_t *bf, const char element[]) {
    bf_k_t k = bf->k;
    bf_m_t Ki[k];
    bf_hashes(element, Ki, k, bf->hash_part);
    bf_k_t i;
    for (i=0; i<k; i++)
        if (!_bf_test_bit(bf->space, Ki[i]))
            return false;
    return true;
}

bool bf_check_then_add(bloom_filter_t *bf, const char element[]) {
    bf_k_t k = bf->k;
    bf_m_t Ki[k];
    bf_hashes(element, Ki, k, bf->hash_part);

    bool present = true;
    bf_k_t i;
    for (i=0; i<k; i++)
        if (!_bf_test_bit(bf->space, Ki[i])) {
            present = false;
            break;
        }
    if (!present)
        for (i=0; i<k; i++)
            _bf_jump_bit(bf->space, Ki[i]);
    return present;
}

bloom_filter_t *bf_create(bf_m_t m, bf_k_t k) {
    //TODO: check m and k
    if (!m || !k)
        return NULL;

    bf_hp_t hp;
    bf_m_t aligned_m;

    _bf_make_m_power_of_two(m, &aligned_m, &hp);

    size_t cells =  (aligned_m + ( BF_BITS_PER_CELL - 1)) / BF_BITS_PER_CELL ;

    bf_cell_t *space = calloc(cells, sizeof(bf_cell_t));
    if (!space)
        return NULL;

    bloom_filter_t *bf = malloc(sizeof(bloom_filter_t));
    if (!bf) {
        free(space);
        return NULL;
    }

    bf->m = aligned_m;
    bf->k = k;
    bf->hash_part = hp;
    bf->space = space;

    return bf;
}

void bf_destroy(bloom_filter_t *bf) {
    if (bf) {
        if (bf->space)
            free(bf->space);
        free(bf);
    }
}
