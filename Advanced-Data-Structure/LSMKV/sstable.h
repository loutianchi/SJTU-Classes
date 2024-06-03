#pragma once

#include "skiplist.h"
#include "bloomfilter.h"
#include "vlog.h"
#include <vector>

namespace SSTable
{
    
    union entr {
        struct KOV {
            uint64_t key;
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
        std::string dir;
        unsigned long long time, num, minKey, maxKey;
        BloomFilter::BloomFilter_type *bloomFilter;
        std::list<std::pair<uint64_t, std::string>> list;
        VLog::VLog_type *vLog;

    public:
        SSTable_type();
        SSTable_type(std::string dir, VLog::VLog_type *vlog);
        SSTable_type(std::vector<entr> &entries, unsigned long long _time);
        explicit SSTable_type(unsigned long long _time, skiplist::skiplist_type *memTable, VLog::VLog_type *vlog);
        ~SSTable_type();
        bool save(std::string dir);
        bool filterCheck(uint64_t key);
        unsigned long long getOffset(uint64_t key);
        unsigned long long getTime(){return time;}
        head getHead(){head h;h.dat.t = time;h.dat.mi = minKey;h.dat.ma = maxKey;h.dat.num = num; return h;}
        entr getItem(int id);
    };

}