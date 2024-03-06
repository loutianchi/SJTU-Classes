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
        // 通过优先队列得出最大的三个组合
        for (std::map<std::string, int>::iterator it = mapBuilding.begin(); 
             it != mapBuilding.end(); ++it) {
			newNode = new hfNode(it->second, it->first);
             q0.push(newNode);
			if (q0.size() > 3) {
				newNode = q0.top();
				q0.pop();
				delete newNode;
			}
		}
		mapBuilding.clear();
        // 将最大的三个字符组合加入 map中
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



##### 

#### 其他文本实验结果

#### 可能的更优策略



###### 一些疑问

1. 在更新了频率表后，由于选取的字符组合有可能相互影响，导致最终选取的并不是使用次数总和最高的三个，对压缩的性能会有影响。