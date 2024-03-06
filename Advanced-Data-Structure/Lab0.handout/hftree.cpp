#include "hftree.h"

#include <queue>
#include <iostream>
#include <cmath>

// const int MULTI_CHAR_NUMBER = 50;

hfTree::hfTree(const std::string &text, const Option op)
{
    std::map <std::string, int> mapBuilding;
    std::priority_queue<hfNode *, std::vector<hfNode *>, multiSelect> q0;
    hfNode *newNode, *chd1, *chd2;

    int MULTI_CHAR_NUMBER = log(text.length()) / log(2) * 2;

    mapBuilding.clear();   
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
			if (q0.size() > MULTI_CHAR_NUMBER) {
				newNode = q0.top();
				q0.pop();
				delete newNode;
			}
		}
		mapBuilding.clear();
        // 将最大的三个字符组合加入 map 中
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

    std::string code = "";
    addTable(root, code, &codingTable);
}

void hfTree::deleteTree(hfNode *now)
{
    if (now == nullptr) return;
    deleteTree(now->leftChild);
    deleteTree(now->rightChild);
    delete now;
    return;
}

hfTree::~hfTree() 
{
    deleteTree(root);
}

void hfTree::addTable(hfNode *now, std::string &code, std::map<std::string, std::string> *mp)
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

std::map<std::string, std::string> hfTree::getCodingTable()
{
    return codingTable;
}