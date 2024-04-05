### HW1

#### 跳表操作

##### 插入

​		按论文中所要求实现，首先找到小于要查询的 key 为关键字的最接近的结点，然后判断他的下一个结点是不是 key （即判断关键字为 key 的结点是否存在）， 若存在则修改原有的 value，否则新加入一个结点，并且为新的结点随机一个高度。

```cpp
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
```

##### 查询

​		与插入的前半部分类似，但无需记录update数组。

```cpp
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
```

##### query_distance函数

​		这部分的要求与前面的 **get** 函数有些不同，他在第一次访问到 key 这个关键字的时候就会直接停止，而不是继续向下层访问其他结点，因此在 get 函数的基础上做了些许修改，代码就不再重复列出了。



#### 运行结果与分析

#### 数据构成

​		首先，由公式 
$$
MaxLevel = \log_{(\frac1p)}N
$$
​		可以得到如下表格：
$$
\begin{array}{c  c |cr}
 &长度 & 50 & 100 & 200 & 500 & 1000 \\
概率 & MaxLevel\\
\hline
0.5000 & & 5.6438 & 6.6438 & 7.6438 & 8.9657 & 9.9657 \\
0.3679 & & 3.9120 & 4.6051 & 5.2983 & 6.2146 & 6.9077 \\
0.2500 & & 2.8219 & 3.3219 & 3.8219 & 4.4828 & 4.9828 \\
0.1250 & & 1.8812 & 2.2146 & 2.5479 & 2.9885 & .32192 \\
\end{array}
$$
​		接下来，按照计算得到的数值设置 Max Level， 并使用测试程序用不同的随机数种子重复测试 10 次取平均值作为这组配置下的实验结果。

​		使用测试程序进行测试，实验的结果如下：
$$
\begin{array}{c c |cr} 
& 长度 & 50 & 100 & 200 & 500 & 1000 \\
概率 & 平均搜索步长 \\
\hline
0.5000 & & 10.39472 & 11.68845 & 12.53754 & 15.58767 & 17.64717 \\
0.3679 & & 8.53733 & 10.26929 & 12.41531 & 14.89009 & 16.46066 \\
0.2500 & & 9.66656 & 11.19019 & 12.61854 & 15.78626 & 17.72272 \\
0.1250 & & 11.89456 & 13.52898 & 16.15402 & 19.68061 & 22.97778 \\

\end{array}
$$
​		根据以上数据作图，得到如下图示：

![image-20240302125355485](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240302125355485.png)

观察图片可以得出以下结论：

1. 在取 p 的值为 1/e 时，平均的搜索步长最短，这也反映了这种情况下的时间复杂度最小。这与论文中所给出的结论（即理论情况）一致。
2. 在取 p 的值为 1/2 或 1/4 时，平均的搜索步长接近，这也与论文中所给出的结论是相符合的。
3. 在总长度 n 增加时，平均搜索步长同步增加，且基本呈对数级的增长。

​		但是在实验的过程中，对最高层数的限制由于只能是一个整数，有可能会对实验结果产生一定的影响，在这里就没有深入的考虑了。