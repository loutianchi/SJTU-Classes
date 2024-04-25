// implement a radix tree that support node compressino and store int32_t values
// each parent node has 4 children, representing 2 bits
#include <cstdint>
#include <algorithm>
#include "tree.hpp"
#define MAXLEVEL 18

class CompressedRadixTree : public Tree
{
    const static int MAXSTEP = 2;
    struct RadixNode
    {
        RadixNode *chd[1 << MAXSTEP], *fa;
        int height, len, chdNum;
        int32_t value;
        RadixNode(RadixNode *father = nullptr) {
            fa = father;
            for (int i = 0; i < (1 << MAXSTEP); i++)
                chd[i] = nullptr;
            chdNum = 0;
        }
        void update() {
            height = 1;
            for (int i = 0; i < (1 << MAXSTEP); i++) {
                if (chd[i] == nullptr) continue;
                height = height > chd[i]->height + 1?:chd[i]->height + 1;
            }
        }
    };
    
    int sizeNum;
    RadixNode *root;
    void deleteTree(RadixNode*);
public:
    CompressedRadixTree();
    ~CompressedRadixTree();
    // basic operation
    void insert(int32_t value);
    void remove(int32_t value);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};