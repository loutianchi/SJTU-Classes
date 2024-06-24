#pragma once

#include <string>

namespace BloomFilter
{
    const int Mod = 65536;
    
    class BloomFilter_type
    {
    private:
        bool flag[Mod];
    public:
        BloomFilter_type();
        BloomFilter_type(char* input);
        ~BloomFilter_type();

        void save(FILE *out);
        void add(uint64_t key);
        bool find(uint64_t key);
    };
    
}