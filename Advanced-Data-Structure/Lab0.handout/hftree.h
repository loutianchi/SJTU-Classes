#ifndef HFTREE_H
#define HFTREE_H

#include <string>
#include <map>

class hfTree
{

private:
/*  
    This is a struct to store nodes of Huffman Tree.
    leftChild and rightChild point to the direct childs of it.
    smallNode stores the minimum text in its subtree.
    cnt stores the value of its subtree.
*/
    struct hfNode
    {
        // 储存了当前节点的左右孩子的信息，以及以当前节点为根的子树中字典序最小的节点地址
        hfNode *leftChild, *rightChild, *smallNode;
        // 储存了叶子节点上的文本内容
        std::string text;
        // 储存了以当前节点为根的子树包含的总出现次数，即当前节点的权值
        int cnt;

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

        // To use the STL priority_queue, overide operator <
        bool operator<(const hfNode& other) const {
            return cnt < other.cnt || (cnt == other.cnt && smallNode->text < other.smallNode->text);
        }
    };

    struct smallFirst
    {
        bool operator()(const hfNode *l, const hfNode *r) const {
            return !((*l) < (*r));
        }
    };
    struct multiSelect
    {
        bool operator()(const hfNode *l, const hfNode *r) const {
            return (l->cnt > r->cnt || (l->cnt == r->cnt && l->smallNode->text < r->smallNode->text));
        }
    };

    // hfTree的根
    hfNode *root;
    std::map<std::string, std::string> codingTable;
    // 储存codingTable的 map
    void deleteTree(hfNode *now);
    void addTable(hfNode *now, std::string &code, std::map<std::string, std::string> *mp);
public:

    enum class Option
    {
        SingleChar,
        MultiChar
    };
    hfTree(const std::string &text, const Option op);
    ~hfTree();
    std::map<std::string, std::string> getCodingTable();
};

#endif