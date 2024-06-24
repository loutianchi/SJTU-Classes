#include "bloomfilter.h"
#include "MurmurHash3.h"

#include <cstring>

namespace BloomFilter
{    
    BloomFilter_type::BloomFilter_type()
    {
        memset(flag, false, sizeof(flag));
    }

    BloomFilter_type::BloomFilter_type(char *input)
    {
        unsigned char c;
        for (int i = 0; i < Mod / 8; i++) {
            c = input[i];
            flag[(i << 3) | 0 ] = (c >> 7) & 1;
            flag[(i << 3) | 1 ] = (c >> 6) & 1;
            flag[(i << 3) | 2 ] = (c >> 5) & 1;
            flag[(i << 3) | 3 ] = (c >> 4) & 1;
            flag[(i << 3) | 4 ] = (c >> 3) & 1;
            flag[(i << 3) | 5 ] = (c >> 2) & 1;
            flag[(i << 3) | 6 ] = (c >> 1) & 1;
            flag[(i << 3) | 7 ] = (c >> 0) & 1;
        }
    }
    
    BloomFilter_type::~BloomFilter_type()
    {
    }
    
    void BloomFilter_type::save(FILE *out)
    {
        unsigned char c;
        for (int i = 0; i < Mod / 8; i++) {
            c = ((flag[(i << 3) | 0 ]) << 7) |
                ((flag[(i << 3) | 1 ]) << 6) |
                ((flag[(i << 3) | 2 ]) << 5) |
                ((flag[(i << 3) | 3 ]) << 4) |
                ((flag[(i << 3) | 4 ]) << 3) |
                ((flag[(i << 3) | 5 ]) << 2) |
                ((flag[(i << 3) | 6 ]) << 1) |
                ((flag[(i << 3) | 7 ]) << 0) ;
            fprintf(out, "%c", c);
        }
    }

    void BloomFilter_type::add(uint64_t key)
    {
        unsigned int hash[4] = {0};
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);
        flag[hash[0] % Mod] = true;
        flag[hash[1] % Mod] = true;
        flag[hash[2] % Mod] = true;
        flag[hash[3] % Mod] = true;
    }

    bool BloomFilter_type::find(uint64_t key)
    {
        unsigned int hash[4] = {0};
        MurmurHash3_x64_128(&key, sizeof(key), 1, hash);
        if (flag[hash[0] % Mod] &&
            flag[hash[1] % Mod] &&
            flag[hash[2] % Mod] &&
            flag[hash[3] % Mod] )
            return true;
        else
            return false;
    }
}