#include <limits.h>

// Bloom-filter parameters
#define hashpart 33
#define m ( 1L << hashpart )
#define k 10

// Network parameters
#define BIND_ADDRESS "0.0.0.0"
#define BIND_PORT 8889

// Internal definitions
#define MIME_TYPE "Content-Type", "text/html; charset=UTF-8"
#define MEGA (1<<20)
#define STR_MAX 1024
#define BITS_PER_CELL (sizeof(bloom_cell) * CHAR_BIT)
