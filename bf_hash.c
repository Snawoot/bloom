#include <stdint.h>
#include <string.h>
#include "md6.h"
#include "bf_types.h"
#include <assert.h>

//Hasher
uint64_t *bf_hashes(const char* bytes, bf_m_t *Ki, bf_k_t k, bf_hp_t hashpart)
{
    md6_state st;

    assert(md6_init(&st, hashpart * k)==0);
    assert(md6_update(&st, (unsigned char *)bytes, strlen(bytes) * CHAR_BIT)==0);
    assert(md6_final(&st, NULL)==0);

    int bit, i, j, n=0;
    for (i=0; i < k; i++) {
        size_t curr_key=0;
        for (j=0; j<hashpart; j++,n++) {
            bit = (st.hashval[n / CHAR_BIT] & ((unsigned char)1 << ((CHAR_BIT - 1) - (n % CHAR_BIT)))) !=0 ? 1 : 0;
            curr_key = (curr_key << 1) | bit;
        }
        Ki[i] = curr_key;
    }
    return Ki;
}
