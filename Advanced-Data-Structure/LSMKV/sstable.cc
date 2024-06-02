
#include "sstable.h"
#include <cstdio>


#include <iostream>
namespace SSTable
{

    SSTable_type::SSTable_type()
    {

    }

    SSTable_type::SSTable_type(std::string dir, VLog::VLog_type *vlog)
    {
        vLog = vlog;
        FILE* in = fopen(dir.c_str(), "r");
        if (nullptr == in) {
            return;
        }
        head h;
        fread(h.c, 32, 1, in);
        time = h.dat.t;
        num = h.dat.num;
        minKey = h.dat.mi;
        maxKey = h.dat.ma;
        char input[BloomFilter::Mod / 8];
        fread(input, BloomFilter::Mod / 8, 1, in);
        bloomFilter = new BloomFilter::BloomFilter_type(input);

        entr data;
        for (int i = 0; i < num; ++i) {
            fread(data.c, 20, 1, in);
            offsetList.push_back(data);
        }
        fclose(in);
    }

    SSTable_type::SSTable_type(unsigned long long _time, skiplist::skiplist_type *memTable, VLog::VLog_type *vlog)
    {
        time = _time;   
        memTable->getAll(list);
        num = list.size();
        auto it = list.begin();
        minKey = (*it).first;
        it = list.end(); --it;
        maxKey = (*it).first;
        bloomFilter = new BloomFilter::BloomFilter_type();
        for (auto item : list) {
            bloomFilter->add(item.first);
        }
        vLog = vlog;
    }   
        
    SSTable_type::~SSTable_type()
    {
        list.clear();
        offsetList.clear();
        delete bloomFilter;
    }

    bool SSTable_type::save(std::string dir)
    {
        head h;
        h.dat.t = time; h.dat.num = num; h.dat.mi = minKey; h.dat.ma = maxKey;

        if (h.dat.num == 0)
            return false;

        dir += '/' + std::to_string(time) + ".sst";
        FILE* out = fopen(dir.c_str(), "w");
        if (nullptr == out) {
            return false;
        }
        fwrite(h.c, 32, 1, out);
        
        bloomFilter->save(out);

        // save KEY OFFSET LEN 
        unsigned long long offset;
        entr data;
        for (auto item : list) {
            if (item.second == "~DELETED~")
                item.second = "";
            offset = vLog->add(item.first, item.second);
            if (-1ull == offset) {
                return false;
            }
            data.kov.key = item.first; data.kov.offset = offset; data.kov.len = item.second.length();
            fwrite(data.c, 20, 1, out);
        }

        fclose(out); out = nullptr;
        return true;
    }

    bool SSTable_type::filterCheck(uint64_t key)
    {
        return bloomFilter->find(key);
    }

    unsigned long long SSTable_type::getOffset(uint64_t key)
    {
        long long l = 0, r = num - 1, mid;
        while (l <= r) {
            mid = l + ((r - l) >> 1);
            if (offsetList[mid].kov.key == key)
                return offsetList[mid].kov.offset;
            if (offsetList[mid].kov.key > key)
                r = mid - 1;
            else
                l = mid + 1;
        }
        return -1ull;
    }

}