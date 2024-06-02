#pragma once

#include "skiplist.h"
#include "bloomfilter.h"
#include "vlog.h"
#include <vector>

namespace SSTable
{
    
    union entr {
        struct KOV {
            unsigned long long key;
            unsigned long long offset;
            unsigned int len;
        }kov;
        char c[20];
    };
    union head {
        struct HVA {
            unsigned long long t, num, mi, ma;
        }dat;
        char c[32];
    };

    class SSTable_type
    {
        unsigned long long time, num, minKey, maxKey;
        BloomFilter::BloomFilter_type *bloomFilter;
        std::list<std::pair<uint64_t, std::string>> list;
        std::vector<entr> offsetList;
        VLog::VLog_type *vLog;

    public:
        SSTable_type();
        SSTable_type(std::string dir, VLog::VLog_type *vlog);
        explicit SSTable_type(unsigned long long _time, skiplist::skiplist_type *memTable, VLog::VLog_type *vlog);
        ~SSTable_type();
        bool save(std::string dir);
        bool filterCheck(uint64_t key);
        unsigned long long getOffset(uint64_t key);
    };

}