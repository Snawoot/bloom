#include <stdint.h>
#include <openssl/sha.h>
#include "bf_types.h"

#define BF_HASH_MAX_WIDTH SHA384_DIGEST_LENGTH * 8
uint64_t *bf_hashes(const char*, bf_m_t *, bf_k_t, bf_hp_t);
