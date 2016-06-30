#include <stdint.h>
#include <string.h>
#include <openssl/sha.h>
#include "bf_types.h"

//Hasher
uint64_t *bf_hashes(const char* bytes, bf_m_t *Ki, bf_k_t k, bf_hp_t hashpart)
{
    unsigned char hashbuf[SHA384_DIGEST_LENGTH];
    SHA384(bytes,  strlen(bytes), hashbuf);

    int bit, i, j, n=0;
    for (i=0; i < k; i++) {
        size_t curr_key=0;
        for (j=0; j<hashpart; j++,n++) {
            bit = (hashbuf[n / CHAR_BIT] & ((unsigned char)1 << ((CHAR_BIT - 1) - (n % CHAR_BIT)))) !=0 ? 1 : 0;
            curr_key = (curr_key << 1) | bit;
        }
        Ki[i] = curr_key;
    }
    return Ki;
}
