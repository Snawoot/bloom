#ifndef BF_TYPES_H_INCLUDED
#define BF_TYPES_H_INCLUDED

#include <limits.h>
#include <stdint.h>

typedef unsigned char bf_cell_t;
typedef uint64_t bf_m_t;
typedef uint8_t bf_k_t;
typedef uint16_t bf_hp_t;
typedef uint16_t bf_hw_t;
typedef struct {
    bf_m_t m;
    bf_k_t k;
    bf_hp_t hash_part;
    bf_hw_t hash_width;
    bf_cell_t *space;
} bloom_filter_t;

#define BF_BITS_PER_CELL (sizeof(bf_cell_t) * CHAR_BIT)

//Legacy
//// Bloom-filter parameters
//#define hashpart 33
//#define m ( 1L << hashpart )
//#define k 10

#endif
