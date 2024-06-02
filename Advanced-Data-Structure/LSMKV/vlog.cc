#include "vlog.h"
#include "utils.h"
#include "kvstore.h"

#include <cstdio>
#include <sys/mman.h>  
#include <fcntl.h>

#include <iostream>

namespace VLog 
{

    VLog_type::VLog_type(const std::string vlogP)
        : vlog(vlogP)
    {
        fd = open(vlog.c_str(), O_RDWR | O_APPEND | O_CREAT, 0644);  
        if (fd < 0)
        {
            perror("open vlog");
        }
        else {
            head = lseek(fd, 0, SEEK_END);
            close(fd);

            tail = utils::seek_data_block(vlog);
            tail = (tail == -1ull ?0 :tail);
            
            fd = open(vlog.c_str(), O_RDWR | O_APPEND, 0644);  
            if (fd < 0)
            {
                perror("open vlog");
            }
            unsigned char c;        
            unsigned short check;
            unsigned long long key;
            unsigned int len;
            char *ptr;

            off_t stOff = (tail >> 12) << 12;
            size_t curSize = PAGE_SIZE;
            ptr = (char*) mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, stOff);
            if (ptr == MAP_FAILED) {
                close(fd);
                perror("mmap");
                return;
            }
            while (tail < head) {
                c = *(unsigned char*)(ptr + tail - stOff);
                if (c != magic) {
                    tail++;
                    continue;
                }
                while (tail + 16 >= stOff + curSize) {
                    curSize += PAGE_SIZE;
                    ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
                }
                memcpy(&check, (ptr + 1 + tail - stOff), sizeof(check));  
                memcpy(&key, (ptr + 3 + tail - stOff), sizeof(key));  
                memcpy(&len, (ptr + 11 + tail - stOff), sizeof(len));
                while (tail + len + 16 >= stOff + curSize) {
                    curSize += PAGE_SIZE;
                    ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
                    if (ptr == MAP_FAILED) {
                        close(fd);
                        return;
                    }
                }
                std::vector<unsigned char> data(len + 12);
                std::copy(ptr + 3 + tail - stOff, ptr + len + 15 + tail - stOff, data.begin());
                if (utils::crc16(data) == check) {
                    break;
                }
                tail++;
                if (tail >= stOff + curSize) {
                    munmap(ptr, curSize);
                    stOff += curSize;
                    curSize = PAGE_SIZE;
                    ptr = (char*) mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, stOff);
                    if (ptr == MAP_FAILED) {
                        close(fd);
                        perror("mmap");
                        return;
                    }
                }
            }
            munmap(ptr, curSize);
        }
    }
    
    VLog_type::~VLog_type()
    {
        close(fd);
    }

    unsigned long long VLog_type::add(const uint64_t key, const std::string &s)
    {
        unsigned int len = s.length();
        std::vector<unsigned char> data(len + 15);

        memcpy(data.data(), &magic, sizeof(magic));  
        memcpy(data.data() + 3, &key, sizeof(key));  
        memcpy(data.data() + 11, &len, sizeof(len));
        memcpy(data.data() + 15, s.c_str(), len);

      //  std::cout << key << " " << s << std::endl;
      //  puts("YES1");

        std::vector<unsigned char> crc_data(data.begin() + 3, data.end());
        unsigned short check = utils::crc16(crc_data);
        memcpy(data.data() + 1, &check, sizeof(check));  

     //   puts("YES2");

        write(fd, data.data(), data.size());

        head += data.size();
        return head - data.size();
    }

    void VLog_type::garbageCollect(const uint64_t chunk_size, KVStore *LSM_TREE)
    {
        unsigned long long last = tail + chunk_size;

        unsigned char c;        
        unsigned short check;
        unsigned long long key;
        std::string value;
        unsigned int len;
        char *ptr;
        
        //printf("ht: %lld %lld\n", head, tail);
        unsigned long long tailIn = tail;
        off_t stOff = (tail >> 12) << 12;
        size_t curSize = PAGE_SIZE;
        ptr = (char*) mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, stOff);
        if (ptr == MAP_FAILED) {
            close(fd);
            perror("mmap");
            return;
        }
        while (tail < last && tail < head) {
            c = *(unsigned char*)(ptr + tail - stOff);
            if (c != magic) {
                tail++;
                continue;
            }
            while (tail + 16 >= stOff + curSize) {
                curSize += PAGE_SIZE;
                ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
            }
            memcpy(&check, (ptr + 1 + tail- stOff), sizeof(check));  
            memcpy(&key, (ptr + 3 + tail - stOff), sizeof(key));  
            memcpy(&len, (ptr + 11 + tail - stOff), sizeof(len));
            while (tail + len + 16 >= stOff + curSize) {
                curSize += PAGE_SIZE;
                ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
            }
            if (ptr == MAP_FAILED) {
                close(fd);
                perror("mmap");
                return;
            }
            std::vector<unsigned char> data(len + 12);
            std::copy(ptr + 3 + tail - stOff, ptr + len + 15 + tail - stOff, data.begin());
            value = std::string(ptr + 15 + tail - stOff, len);
            
            if (utils::crc16(data) == check) {
                // find in lsm, check the offset, if it is same, insert again
                if (LSM_TREE->checkOffset(key, tail)) {
                    LSM_TREE->put(key, value);
                }
            }
            tail += len + 15;
            if (tail >= stOff + curSize) {
                munmap(ptr, curSize);
                stOff += curSize;
                curSize = PAGE_SIZE;
                ptr = (char*) mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, stOff);
                if (ptr == MAP_FAILED) {
                    close(fd);
                    perror("mmap");
                    return;
                }
            }
        }
        munmap(ptr, curSize);
        if (tail < last)
        {            
            long long num = last - tail;
            char *buffer = (char *)malloc(num);
            if (buffer == NULL) {
                perror("Failed to allocate buffer");
                close(fd);
                return;
            }
            memset(buffer, '\0', num); 
            ssize_t written = write(fd, buffer, num);
            if (written == -1) {
                perror("Failed to write to file");
                free(buffer);
                close(fd);
                return;
            }
            free(buffer);
            head = tail = last;
        };
        utils::de_alloc_file(vlog, tailIn, tail - tailIn);;
        LSM_TREE->save();
    }

    std::string VLog_type::getByOffset(const uint64_t offset)
    {
        off_t stOff = (offset >> 12) << 12;
        size_t curSize = PAGE_SIZE;
        char *ptr = (char*) mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, stOff);
        unsigned char c = *(unsigned char*)(ptr + offset - stOff);
        if (c != magic) {
            return "";
        }
        std::string value;
        unsigned int len;
        while (offset + 16 >= stOff + curSize) {
            curSize += PAGE_SIZE;
            ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
        }
        memcpy(&len, (ptr + 11 + offset - stOff), sizeof(len));
        while (offset + len + 15 >= stOff + curSize) {
            curSize += PAGE_SIZE;
            ptr = (char*) mremap(ptr, curSize - PAGE_SIZE, curSize, PROT_READ, MAP_SHARED);
        }
        if (ptr == MAP_FAILED) {
            close(fd);
            perror("mmap");
            return "";
        }
        value = std::string(ptr + 15 + offset - stOff, len);
        munmap(ptr, curSize);
        return value;
    }
}