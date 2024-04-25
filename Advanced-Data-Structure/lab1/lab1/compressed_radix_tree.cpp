#include "compressed_radix_tree.hpp"
#include <cstring>
#include <cassert>
#include <cmath>
#include <iostream>
#define MASK 0x3
#define MaxLen 32

CompressedRadixTree::CompressedRadixTree()
{
    root = new RadixNode;
    root->len = 0;
    root->height = 1;
    sizeNum = 1;
}

void CompressedRadixTree::deleteTree(RadixNode *now)
{
    for (int i = 0; i < (1 << MAXSTEP); i++) {
        if (now->chd[i] == nullptr) continue;
        deleteTree(now->chd[i]);
        now->chd[i] = nullptr;
    }
    delete now;
}

CompressedRadixTree::~CompressedRadixTree()
{
    delete(root);
}

void CompressedRadixTree::insert(int32_t value)
{
    int mask = MASK, mov = MaxLen;
    int id;
    RadixNode *now = root, *lst;
    while (mov > 0) {
        id = (value >> (mov - MAXSTEP)) & mask;
        if (now->chd[id] == nullptr) {
            // Simple add.
            now->chd[id] = new RadixNode(now);
            now->chd[id]->value = value & ((1ull << mov) - 1);
            now->chd[id]->len = mov;
            now->chd[id]->height = 1;
            now->chdNum++;
            lst = now->chd[id];
            while (now != nullptr) {
                now->height = std::max(now->height, lst->height + 1);
                lst = now;
                now = now->fa;
            }
            this->sizeNum++;
            return;
        }
        // Already exist, but not match. Split it.
        if (now->chd[id]->value != ((value >> (mov - now->chd[id]->len)) & ((1ull << now->chd[id]->len) - 1)))
            goto SPLIT;
        now = now->chd[id];
        mov -= now->len;
    }
    // Exist the exactly same value.
    return;
SPLIT:
    now = now->chd[id];
    int32_t va1 = now->value, va2 = value & ((1ull << mov) - 1);
    int len1 = now->len, len2 = mov, sameLen = MAXSTEP;
    do {
        sameLen += MAXSTEP;
    }while(sameLen <= len1 && (va1 >> (len1 - sameLen)) == (va2 >> (len2 - sameLen)));
    sameLen -= MAXSTEP;
    // Split
    RadixNode *newFa;
    newFa = new RadixNode(now->fa);
    newFa->fa->chd[va1 >> (now->len - MAXSTEP)] = newFa;
    newFa->value = va1 >> (len1 - sameLen);
    newFa->len = sameLen; 
    newFa->chdNum += 2;

    len1 -= sameLen; va1 = va1 & ((1ull << len1) - 1); 
    len2 -= sameLen; va2 = va2 & ((1ull << len2) - 1);

    id = (va2 >> (len2 - MAXSTEP)) & mask;
    newFa->chd[id] = new RadixNode(newFa);
    newFa->chd[id]->value = va2;
    newFa->chd[id]->len = len2;
    newFa->chd[id]->height = 1;
    
    id = (va1 >> (len1 - MAXSTEP)) & mask;
    now->fa = newFa;
    newFa->chd[id] = now;
    newFa->chd[id]->value = va1;
    newFa->chd[id]->len = len1;

    lst = now;
    now = newFa;
    while (now != nullptr) {
        now->height = std::max(now->height, lst->height + 1);
        lst = now;
        now = now->fa;
    }
    this->sizeNum += 2;
}

void CompressedRadixTree::remove(int32_t value)
{
    int mask = MASK, mov = MaxLen;
    int id;
    RadixNode *now = root, *nxt, *cd;
    while (mov > 0) {
        id = (value >> (mov - MAXSTEP)) & mask;
        // Not match.
        if (now->chd[id] == nullptr) {
            return;
        }
        // Already exist.
        if (now->chd[id]->value != ((value >> (mov - now->chd[id]->len)) & ((1ull << now->chd[id]->len) - 1))) {
            return;
        }
        now = now->chd[id];
        mov -= now->len;
    }
    while (now != root) {
        nxt = now->fa;
        id = (now->value >> (now->len - MAXSTEP)) & mask;
        if (now->chdNum == 0) {
            nxt->chd[id] = nullptr;
            nxt->chdNum--;
            this->sizeNum--;
            delete now;
        }
        else if (now->chdNum == 1) {
            for (int i = 0; i < (1 << MAXSTEP); i++)
                if (now->chd[i] != nullptr) {
                    cd = now->chd[i];
                    break;
                }
            cd->fa = nxt;
            cd->value = cd->value | (now->value << cd->len);
            cd->len += now->len;
            nxt->chd[id] = cd;
            this->sizeNum--;
            delete now;
        }
        nxt->update();
        now = nxt;
    }
}

bool CompressedRadixTree::find(int32_t value)
{
    int mask = MASK, mov = MaxLen;
    int id;
    RadixNode *now = root;
    while (mov > 0) {
        id = (value >> (mov - MAXSTEP)) & mask;
        // Not match.
        if (now->chd[id] == nullptr) {
            return false;
        }
        // Already exist.
        if (now->chd[id]->value != ((value >> (mov - now->chd[id]->len)) & ((1ull << now->chd[id]->len) - 1)))
            return false;
        now = now->chd[id];
        mov -= now->len;
    }
    return true;
}

uint32_t CompressedRadixTree::size()
{
    return sizeNum;
}

uint32_t CompressedRadixTree::height()
{
   return root->height;
}
