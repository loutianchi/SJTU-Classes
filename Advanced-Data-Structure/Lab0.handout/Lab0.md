### Lab0

#### 哈夫曼树的实现

##### 哈夫曼树的成员变量

​		在哈夫曼树（hfTree) 这个类中，定义了一个新的 struct, 命名为 hfNode， 这个结构体是用于储存哈夫曼树中的每个节点的具体数据的，实现如下。

``` cpp
    struct hfNode
    {
        // 储存了当前节点的左右孩子的信息，以及以当前节点为根的子树中字典序最小的节点地址
        hfNode *leftChild, *rightChild, *smallNode;
        // 储存了叶子节点上的文本内容
        std::string text;
        // 储存了以当前节点为根的子树包含的总出现次数，即当前节点的权值
        int cnt;
    }
```

​		此外，为了方便地构造叶子节点与非叶子节点，实现了两个构造函数，如下。

```cpp
	// 叶子节点的构造函数，times代表出现次数，s保存了文本		
	hfNode (int times = 0, const std::string s = "") {
		cnt = times;
		text = s;
         smallNode = this;
         leftChild = rightChild = nullptr;
    }

	// 非叶子节点的构造方式，chd1、chd2为要构造节点的两个孩子
	hfNode (hfNode *chd1, hfNode *chd2) {
        cnt = chd1->cnt + chd2->cnt;
        if (chd1->smallNode->text < chd2->smallNode->text)
            smallNode = chd1->smallNode;
        else
            smallNode = chd2->smallNode;
        if ((*chd1) < (*chd2) ) {
            leftChild = chd1;
            rightChild = chd2;
        }
        else {
            leftChild = chd2;
            rightChild = chd1;
        }
    }
```

​		为了方便比较两个节点的大小，重载了 < 运算符。

```cpp
	bool operator<(const hfNode& other) const {
		return cnt < other.cnt || 
         	(cnt == other.cnt && smallNode->text < other.smallNode->text);
 	}
```

​		另外，也定义了几个其他成员变量如下。

```cpp
    // hfTree的根
	hfNode *root;
	// 储存codingTable的 map
    std::map<std::string, std::string> codingTable;
```

##### 建树过程

​		首先，遍历整段文本，统计每个字符出现的次数，并保存在 std::map 中。(当然分了要求的两种 Option)

```cpp
    // MultiChar的情况
	if (op == Option::MultiChar) {
        // 首先统计出所有双字符组合的出现次数
    	for (int i=0; i < text.length() - 1; i++) 
    		mapBuilding[text.substr(i,2)]++;
        // 通过优先队列得出最大的 MULTI_CHAR_NUMBER 个组合
        for (std::map<std::string, int>::iterator it = mapBuilding.begin(); 
             it != mapBuilding.end(); ++it) {
			newNode = new hfNode(it->second, it->first);
             q0.push(newNode);
			if (q0.size() > MULTI_CHAR_NUMBER) {
				newNode = q0.top();
				q0.pop();
				delete newNode;
			}
		}
		mapBuilding.clear();
        // 将最大的 MULTI_CHAR_NUMBER 个字符组合加入 map中
		while (!q0.empty()) {
			newNode = q0.top();q0.pop();
			mapBuilding[newNode->text] = 0;
		}
	}
	
	// 单字母的情况
    std::string subs;
	// 与上面的区别是排除了已经是双字符组合中的单字母
    for (int i=0; i < text.length(); i++) {
        if (i < text.length() - 1) {
            subs = text.substr(i, 2);
            if (mapBuilding.find(subs) != mapBuilding.end()) {
                i++;
                mapBuilding[subs]++;
                continue;
            }
        }
        subs = text.substr(i,1);
        mapBuilding[subs]++;
    }
```

​		而后，通过将 mapBuilding 中的所有元素存入优先队列中，通过每次取出最 top 的两个元素，并将其合并生成一个新的节点插入优先队列中，最后剩下的节点就是根，保存在 hfTree 中的 root。在建完树以后，便将codingTable 生成并存入 hfTree 中。生成 hfTree 与递归生成 codingTable 的代码如下。

```cpp
	// 首先将所有元素加入优先队列
	std::priority_queue<hfNode *, std::vector<hfNode *>, smallFirst> q;
    for (std::map<std::string, int>::iterator it = mapBuilding.begin(); 
         it != mapBuilding.end(); ++it) {
        newNode = new hfNode(it->second, it->first);
        q.push(newNode);
    }
	// 留下最后一个元素作为根
    while (q.size() > 1) {
        chd1 = q.top(); q.pop();
        chd2 = q.top(); q.pop();
        newNode = new hfNode(chd1, chd2);
        q.push(newNode);
    }
    root = q.top();
    q.pop();
```

```cpp
void hfTree::addTable
    (hfNode *now, std::string &code, std::map<std::string, std::string> *mp)
{
    // 若已经是叶子节点，存入表中，返回。
    if (now->leftChild == nullptr && now->rightChild == nullptr) {
        (*mp)[now->text] = code;
        return;
    }
    // 左子树在末尾加 0
    code += "0";
    addTable(now->leftChild, code, mp);
    code.erase(code.length() - 1, 1);

    // 右子树在末尾加 1
    code += "1";
    addTable(now->rightChild, code, mp);
    code.erase(code.length() - 1, 1);
}
```

##### 压缩中功能函数的实现

​		codingTable2String函数简单的遍历了 codingTable。parseText 函数，使用 ifstream 读入文档，不再赘述。output 函数， 同样使用 ofstream 输出因此也列出代码。loadCodingTable 比较复杂，代码如下。

```cpp
void loadCodingTable
    (const std::string &input, std::map<std::string, std::string> &codingTable)
    {
    	// 通过 ifstream 读入存在文件中的 codingTable
        std::ifstream in(input);
        if (!in.is_open()) {
            puts("File name is wrong");
            return;
        }
        std::string content;
        content = "";
        char c;
        while (in.get(c)) {
            content += c;
        }
        in.close();

    	// 通过寻找规律，发现当出现不在第一个的连续的空格加数字0/1，
    	// 表明这一条single或multichar的文本结束
        std::string ch, code;
        int pt = 0;
        while (pt < content.length()) {
            code = "";
            ch = content.substr(pt, 1); pt++;
            if (content.substr(pt, 2) != " 0" && content.substr(pt, 2) != " 1") {
                ch = ch + content[pt];
                pt++;
            }
            pt++;
            while (content[pt] == '0' || content[pt] == '1') {
                code = code + content[pt];
                pt++;
            }
            pt++;
            codingTable[ch] = code;
        }
    }

```

​		此外，压缩部分的代码也较为复杂，代码如下，解释在注释部分。

```cpp
    std::string compress
        (const std::map<std::string, std::string> &codingTable, const std::string &text)
    {
        std::string result = "";

        char c = (char) 0;
        unsigned long long len = 0;
        std::string ch, code;
        
        // 遍历整个字符串
        for (int i = 0; i < text.length(); i++) {
            // 首先判断是否是属于multichar的情况
            if (i < text.length() - 1) {
                ch = text.substr(i, 2);
                i++;
            }
            else
                ch = text.substr(i, 1);
            // 若不是multichar的情况，则取一个字符（一定满足）
            if (codingTable.find(ch) == codingTable.end()) {
                i--;
                ch = text.substr(i,1);
            }
            auto it = codingTable.find(ch);
            // 如果发现字符（或字符组合）不存在，则报错。
            // 但是由于codingTable是为当前文本生成的，这种情况不应该存在）
            assert(it != codingTable.end());
            if (it != codingTable.end())
                code = it->second;
            for (int k = 0; k < code.length(); k++) {
                len++;
                // 按位插入到后面
                c = (c << 1) | (code[k]=='0'?0:1);
                if (len % 8 == 0) {
                    result += c;
                    c = (char) 0;
                }
            }
        }
        // 在结尾补全 8 位并转为字符
        if (len % 8 != 0) {
           c = (c << (8 - len % 8));
           result += c;
        }
        
        // 生成总字符数的小端表示，并加到结果的前面
        std::string lengthLittleEndian = "";
        for (int i = 1; i <= 8; i++) {
            c = (char) len & 0xFF;
            lengthLittleEndian = lengthLittleEndian + c;
            len >>= 8;
        }
        result = lengthLittleEndian + result;
        return result;
    }
```

---

#### 文本实验结果

##### 样例文本实验

​		实验结果如下，数字代表的是压缩后的文本长度。
$$
\begin{array}{c | cr}
样例名 & a & b & c & d\\ 
\hline 
sin & 33 & 448 & 638 & 5390 \\
mul & 24 & 429 & 614 & 5292 \\
\end{array}
$$
​		观察上面的表格，可以发现双字符的使用确实降低了文本的长度，而且降低的数量随文本长度增加而增加，但是降低的比率随着文本总长度的增加而减小，猜测是由于双字符数量达到上限（3）导致的。

##### 自选文本实验

​		以下数据以 bit 计。
$$
\begin{array}{c | c}
样例名 & GPT\ novel& The \ Old \ Man \ and \ the \ Sea  & 1984 & Harry \ Potter(3)& Gone \ with \ the \ Wind\\ 
\hline 
init & 17832 & 1105256 & 4745000 & 5426664 & 19057240\\
sin & 9882 & 611785 & 2659150 & 3203617 & 10856509 \\
mul & 9736 & 603809 & 2656197 & 3186411 & 10774306 \\
\end{array}
$$
​		将获得的原始数据进行处理，得到：
$$
\begin{array}{c | c}
样例名 & GPT\ novel& The \ Old \ Man \ and \ the \ Sea  & 1984 & Harry \ Potter(3)& Gone \ with \ the \ Wind\\ 
\hline 
压缩率(sin) & 44.58\% & 44.65\% & 43.96\% & 40.97\% & 43.03\% \\
压缩率(mul) & 45.40\% & 45.37\% & 44.02\% & 41.28\% & 43.46\% \\
mul相对于sin的提高 & 1.48\% & 1.30\% & 1.11\% & 0.54\% & 0.76\% \\
\end{array}
$$
​		从以上的实验结果可以看出，整体的压缩率在 40% ~ 45%，且使用multiChar的方法的压缩率高于singleChar的方法。在文本量较大时，multiChar 的压缩方式并没有很好的提高这个压缩率，且文本量越大，提高的幅度就越小。此外，随着字符数量的上升，压缩率有所下降，但是不是非常明显。

##### 当前算法的一些问题及可能的解决方法

###### 字符组合数量不够带来压缩率的限制

​		在文本量很大的情况下，仅仅使用 3 个可能的 multiChar 组合会限制 multiChar 对压缩率的提高。

​		要解决这个问题，我们可以通过使用更长的 multiChar 组合来提高压缩率，也可以根据文本的长度来设置最大的 multiChar 的数量，由此提高压缩率。

​		对改变最大 multiChar 的数量来提高压缩率的方法，我做了如下实验：

1. 将最大数量改为 5
2. 将最大数量改为 20
3. 将最大数量改为 50
4. 将最大数量改为 

$$
log_2(length) * 2
$$

​		对为什么要对这个数量动态调整，是因为在文本很短的时候，如果 multiChar 的数量过大，会使另一个索引文件的体积过大，因此使压缩的过程适得其反。

​		得到的数据如下：

| 样例 | GPT novel | The Old Man and the Sea | 1984 | Harry Porter(3) | Gone With the Wind |
| ------ | --------- | ----------------------- | ---- | --------------------------------------------------------- | ------ |
| 5 | 9756 | 600340 | 2636469 | 3080571 |10730046|
| 20 | 9613 | 585397 | 2582282 | 3018025 |10274127|
| 50 | 9255 | 568785 | 2503330 | 2948659 |10115020|
| log | 9579 | 573282 | 2538554 | 2967326 |10130259|

​		可以发现，从 3 扩大到 5 再到 20 这个阶段，文本量的减少比较显著，而从 20 到 50 即使增加了 30 个可以选择的文本，压缩率的提高仍不是很高。因此做出了一个随初始文本量多少而改变的函数来选择一个最大数量，使其选择的过程耗时，index 文件与压缩率都获得一个较好的结果。



###### 字符组合选取中的问题

​		在更新了频率表后，由于选取的字符组合有可能相互影响（比如样例中的" i"的选取导致"is"数量的减少），导致最终选取的并不是使用次数总和最高的三个，对压缩的性能会有影响。但是在实际运行的情况中，在大规模的文本量下，影响并不是非常大。

​		想要解决这个问题，可以在选择的过程中，对选择的字符进行筛选，比如以某个特定的概率来决定当前已经出现过的字符是不是会再次被选中，但是这个方法还是不一定可以排除这种问题。



###### 其他可能可以优化的方法

​		将一个大的文件分为不同的部分，由于不同部分的主题可能是不一样的，不同的词汇出现的频率也会因此有所变化，为不同的部分建立不同的哈夫曼树，可以进一步提高压缩率。但是相应的，压缩的索引文档会变大。

<br>

<br>

<br>



---

#### 可能的更优策略

​		使用 multiChar 的算法，并根据输入的文本量动态调整最大数量的多字符组合，同时在文本量较大时使用更长的 multiChar 组合，从而提高压缩率。这样可能带来的问题是时间复杂度的提高，导致压缩时的耗时更长。此外，对非常长的文本可以将其切分为多个部分，并对不同部分计算出不同的哈夫曼树，从而进一步提高压缩率，同时也可以降低哈夫曼树生成的时间。