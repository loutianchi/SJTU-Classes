#include "skiplist.h"
#include <optional>

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

namespace skiplist {

int skiplist_type::randomLevel()
{
    int res = 1;
    while ( (rand() * 1.0 / RAND_MAX) < possibility && res < MAX_LEVEL - 1)
        res++;
    return res;
}

skiplist_type::skiplist_type(double p)
{
    srand((unsigned int)time(NULL));
    possibility = p;
    max_level = 1;
    total = 0;
    key_type k = 0; value_type v = "";
    first = new skiplistNode_type(k, v);
    last = new skiplistNode_type(k, v);
    for (int i = MAX_LEVEL - 1; i >= 0; i--)
        first->forward[i] = last;
}


// 释放内存
skiplist_type::~skiplist_type() {
    skiplistNode_type *now = first, *nxt;
    while (now != last) {
        nxt = now->forward[0];
        delete now;
        now = nxt;
    }
    delete now;
}

void skiplist_type::put(key_type key, const value_type &val) 
{
    // 寻找小于且最接近key的结点
    skiplistNode_type *now = first;
    for (int i = max_level; i >= 1; i--) {
        while (now->forward[i] != last && now->forward[i]->key < key)
            now = now->forward[i];
        update[i] = now;
    }
    now = now->forward[1];
    // 判断关键字为key的结点是否存在
    if (now != last && now->key == key) {
        now->value = val;
        return;
    }
    // 新建一个结点，随机生成层数并插入
    skiplistNode_type *newNode = new skiplistNode_type(key, val);
    total++;
    int v = randomLevel();
    if (v > max_level) {
        for (int i = max_level + 1; i <= v; i++)
            update[i] = first;
        max_level = v;
    }
    // 更新forward的信息
    for (int i = 1; i <= v; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
}

std::string skiplist_type::get(key_type key) const 
{
    // 寻找需求的结点
    skiplistNode_type *now = first;
    for (int i = max_level; i >= 1; i--) 
        while (now->forward[i] != last && now->forward[i]->key < key)
            now = now->forward[i];
    now = now->forward[1];
    if (now != last && now->key == key)
        return now->value;
    // failure的情况
    else
        return "";
}

int skiplist_type::del(key_type key) {
    skiplistNode_type *now = first;
    for (int i = max_level; i >= 1; i--) 
        while (now->forward[i] != last && now->forward[i]->key < key)
            now = now->forward[i];
    skiplistNode_type *toBeDeleted = now->forward[1];
    if (toBeDeleted != last && toBeDeleted->key == key) {
        if (toBeDeleted->value != "~DELETED~")
            toBeDeleted->value = "~DELETED~";
        // 最新状态是被删除，不能再删一次
        else
            return -1;
        return 1;
    }
    // 要在SSTable中找
    else {
        return 0;
    }
}

void skiplist_type::scan(key_type st, key_type ed, std::list<std::pair<key_type, value_type> > &res) {
    skiplistNode_type *now = first;
    for (int i = max_level; i >= 1; i--) 
        while (now->forward[i] != last && now->forward[i]->key < st)
            now = now->forward[i];
        while (now->forward[1] != last && now->forward[1]->key <= ed) {
        now = now->forward[1];
        res.push_back(std::make_pair(now->key, now->value));
    }
}

void skiplist_type::getAll(std::list<std::pair<key_type, value_type> > &res)
{
    skiplistNode_type *now = first;
    while (now->forward[1] != last) {
        now = now->forward[1];
        res.push_back(std::make_pair(now->key, now->value));
    }
}

bool skiplist_type::isEmpty()
{
    return (first->forward[1] == last);
}

bool skiplist_type::isFull()
{
    return (total >= MAX_NUM);
}

} // namespace skiplist
