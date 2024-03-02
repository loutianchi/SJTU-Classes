#include "skiplist.h"
#include <optional>

#include <cstdlib>
#include <ctime>
#include <cmath>

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
    key_type k = 0; value_type v = "";
    first = new skiplistNode_type(k, v);
    last = new skiplistNode_type(k, v);
    for (int i = MAX_LEVEL - 1; i >= 0; i--)
        first->forward[i] = last;
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

int skiplist_type::query_distance(key_type key) const 
{
    int steps = 0;
    skiplistNode_type *now = first;
    for (int i = max_level; i >= 1; i--) {
        steps++;
        while (now->forward[i] != last && now->forward[i]->key < key) {
            now = now->forward[i];
            steps++;
        }
        if (now->forward[i] != last && now->forward[i]->key == key)
            return steps + 1;
    }
    return steps;
}

} // namespace skiplist
