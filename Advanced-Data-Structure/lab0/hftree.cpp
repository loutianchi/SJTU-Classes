#include "hftree.h"

#include <queue>
#include <iostream>

const int MULTI_CHAR_NUMBER = 3;

hfTree::hfTree(const std::string &text, const Option op)
{
    std::map <std::string, int> mapBuilding;
    mapBuilding.clear();
    if (op == Option::MultiChar) {
        for (int i=0; i < text.length() - 1; i++) 
            mapBuilding[text.substr(i,2)]++;
        for (std::map<std::string, int>::iterator it = mapBuilding.begin(); it != mapBuilding.end(); ++it) {
            //TODO
        }
    }

    for (int i=0; i < text.length(); i++) {
        //TODO
        mapBuilding[text.substr(i,1)]++;
    }
    
    std::priority_queue<hfNode *, std::vector<hfNode *>, smallFirst> q;
    hfNode *newNode, *chd1, *chd2;
    for (std::map<std::string, int>::iterator it = mapBuilding.begin(); it != mapBuilding.end(); ++it) {
        newNode = new hfNode(it->second, it->first);
        q.push(newNode);
    }

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
    if (now->leftChild == nullptr && now->rightChild == nullptr) {
        (*mp)[now->text] = code;
        return;
    }
    code += "0";
    addTable(now->leftChild, code, mp);
    code.erase(code.length() - 1, 1);

    code += "1";
    addTable(now->rightChild, code, mp);
    code.erase(code.length() - 1, 1);

}

std::map<std::string, std::string> hfTree::getCodingTable()
{
    return codingTable;
}