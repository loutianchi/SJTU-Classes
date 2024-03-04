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
        hfNode *leftChild, *rightChild, *smallNode;
        std::string text;
        int cnt;
        hfNode (int times = 0, const std::string s = "") {
            cnt = times;
            text = s;
            smallNode = this;
            leftChild = rightChild = nullptr;
        }
        // To use the STL priority_queue, overide operator <
        bool operator<(const hfNode& other) const {
            return cnt < other.cnt || (cnt == other.cnt && smallNode->text < other.smallNode->text);
        }
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
    };
    struct smallFirst
    {
        bool operator()(const hfNode *l, const hfNode *r) const {
            return !((*l) < (*r));
        }
    };
    

    hfNode *root;
    std::map<std::string, std::string> codingTable;
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