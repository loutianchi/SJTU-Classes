#include <cstdint>
#include "tree.hpp"


class RadixTree : public Tree
{
    const static int MAXSTEP = 2;
    struct RadixNode
    {
        RadixNode *chd[1 << MAXSTEP], *fa;
        int chdNum;
        RadixNode(RadixNode *father = nullptr) {
            fa = father;
            for (int i = 0; i < (1 << MAXSTEP); i++)
                chd[i] = nullptr;
            chdNum = 0;
        }
    };
    
    int heightNum;
    int sizeNum;
    RadixNode *root;
    void deleteTree(RadixNode*);
public:
    RadixTree();
    ~RadixTree();
    // basic operation
    void insert(int32_t value);
    void remove(int32_t value);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};
