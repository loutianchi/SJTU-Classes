#include "radix_tree.hpp"
#include <cmath>
#define MASK 0x3
#define MaxLen 32
#include <iostream>

RadixTree::RadixTree()
{
    root = new RadixNode;
    heightNum = 1;
    sizeNum = 1;
}

void RadixTree::deleteTree(RadixNode *now)
{
    for (int i = 0; i < (1 << MAXSTEP); i++) {
        if (now->chd[i] == nullptr) continue;
        deleteTree(now->chd[i]);
        now->chd[i] = nullptr;
    }
    delete now;
}
RadixTree::~RadixTree()
{
    deleteTree(root);
}

void RadixTree::insert(int32_t value)
{
    int mask = MASK, mov = MaxLen, h = 1;
    int id;
    RadixNode *now = root;
    while (mov > 0) {
        mov -= MAXSTEP;
        h++;
        id = (value >> mov) & mask;
        if (now->chd[id] == nullptr) {
            now->chd[id] = new RadixNode(now);
            now->chdNum++;
            this->sizeNum++;
        }
        now = now->chd[id];
    }
    this->heightNum = h;
}

void RadixTree::remove(int32_t value)
{
    int mask = MASK, mov = MaxLen;
    int id;
    RadixNode *now = root, *nxt;
    while (mov > 0) {
        mov -= MAXSTEP;
        id = (value >> mov) & mask;
        if (now->chd[id] == nullptr)
            return;
        now = now->chd[id];
    }
    while (now != root) {
        id = (value >> mov) & mask;
        nxt = now->fa;
        if (now->chdNum == 0) {
            nxt->chd[id] = nullptr;
            nxt->chdNum--;
            this->sizeNum--;
            delete now;
        }
        else
            break;
        mov += MAXSTEP;
        now = nxt;
    }
    if (root->chdNum == 0)
        this->heightNum = 1;
}

bool RadixTree::find(int32_t value)
{
    int mask = MASK, mov = MaxLen;
    int id;
    RadixNode *now = root, *nxt;
    while (mov > 0) {
        mov -= MAXSTEP;
        id = (value >> mov) & mask;
        if (now->chd[id] == nullptr)
            return false;
        now = now->chd[id];
    }
    return true;
}

uint32_t RadixTree::size()
{
    return sizeNum;
}

uint32_t RadixTree::height()
{
    return heightNum;
}
