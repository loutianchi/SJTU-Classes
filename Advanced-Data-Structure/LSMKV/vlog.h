#pragma once

#include <string>

class KVStore;

namespace VLog
{
    const unsigned char magic = 0xFF;
    const int PAGESIZE = 4096;
    const int HEAD_LEN = 15;
    const int CHECK_LEN = 12;

    class VLog_type
    {
    private:
        const std::string vlog;
        int fd;
        unsigned long long tail, head;
    public:
        VLog_type(const std::string vlogP);
        ~VLog_type();
        unsigned long long add(const uint64_t key, const std::string &s);
        void garbageCollect(const uint64_t chunk_size, KVStore *LSM_TREE);
        std::string getByOffset(const uint64_t offset);
    };
    

}