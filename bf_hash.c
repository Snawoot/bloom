#include <stdint.h>
#include <string.h>
#include <openssl/evp.h>
#include "bf_types.h"

//Hasher
uint64_t *bf_hashes(const char* bytes, bf_m_t *Ki, bf_k_t k, bf_hp_t hashpart)
{
    int keylen = (hashpart * k + CHAR_BIT - 1) / CHAR_BIT;
    unsigned char hashbuf[keylen];
    PKCS5_PBKDF2_HMAC_SHA1(bytes, -1, NULL, 0, 1, keylen, hashbuf);

    int bit, i, j, n=0;
    for (i=0; i < k; i++) {
        bf_m_t curr_key=0;
        for (j=0; j<hashpart; j++,n++) {
            bit = (hashbuf[n / CHAR_BIT] & ((unsigned char)1 << ((CHAR_BIT - 1) - (n % CHAR_BIT)))) !=0 ? 1 : 0;
            curr_key = (curr_key << 1) | bit;
        }
        Ki[i] = curr_key;
    }
    return Ki;
}
