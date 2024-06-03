
#include "sstable.h"
#include "utils.h"

#include <cstdio>
#include <sys/mman.h>

#include <iostream>
namespace SSTable
{

    SSTable_type::SSTable_type()
    {
        bloomFilter = new BloomFilter::BloomFilter_type();
    }

    SSTable_type::SSTable_type(std::string dir, VLog::VLog_type *vlog) {
        this->dir = dir;
        vLog = vlog;
        int fd = open(dir.c_str(), O_RDONLY);
        if (fd == -1) {
            return;
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            return;
        }

        size_t file_size = st.st_size;
        char *map = (char *)mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        char *sta = map;
        if (map == MAP_FAILED) {
            close(fd);
            return;
        }

        head h;
        memcpy(h.c, map, 32);
        time = h.dat.t;
        num = h.dat.num;
        minKey = h.dat.mi;
        maxKey = h.dat.ma;
        map += 32;

        bloomFilter = new BloomFilter::BloomFilter_type(map);
        map += (BloomFilter::Mod / 8);
        
        munmap(sta, file_size);
        close(fd);
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
        delete bloomFilter;
    }

    bool SSTable_type::save(std::string dir)
    {
        head h;
        h.dat.t = time; h.dat.num = num; h.dat.mi = minKey; h.dat.ma = maxKey;

        if (h.dat.num == 0)
            return false;

        dir += '/' + std::to_string(time) + ".sst";
        this->dir = dir;
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
        int fd = open(dir.c_str(), O_RDONLY);
        if (fd == -1) {
            return -1ull;
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            return -1ull;
        }

        unsigned long long offset = 32 + BloomFilter::Mod / 8;
        size_t file_size = st.st_size;
        char *map = (char *)mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        char *sta = map;
        if (map == MAP_FAILED) {
            close(fd);
            return -1ull;
        }
        map += offset;
        
        entr data;

        long long l = 0, r = num - 1, mid;
        while (l <= r) {
            mid = l + ((r - l) >> 1);
            memcpy(data.c, map + (mid * 20), 20);
            if (data.kov.key == key) {
                munmap(sta, file_size);
                close(fd);
                return data.kov.offset;
            }
            if (data.kov.key > key)
                r = mid - 1;
            else
                l = mid + 1;
        }
        
        munmap(sta, file_size);
        close(fd);
        return -1ull;
    }
    
    entr SSTable_type::getItem(int id)
    {
        entr invalid;
        invalid.kov.key = -1ull;
        invalid.kov.offset = -1ull;
        invalid.kov.len = -1ull;

        int fd = open(dir.c_str(), O_RDONLY);
        if (fd == -1) {
            return invalid;
        }

        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            return invalid;
        }

        unsigned long long offset = 32 + BloomFilter::Mod / 8;
        size_t file_size = st.st_size;
        char *map = (char *)mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        char *sta = map;
        if (map == MAP_FAILED) {
            close(fd);
            return invalid;
        }
        map += offset;
            
        entr data;
        memcpy(data.c, map + (id * 20), 20);
        
        munmap(sta, file_size);
        close(fd);
        return data;
    }


}