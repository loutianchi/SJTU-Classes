#include <iostream>
#include "rbtree.h"
#include <cstdlib>

bool f[20000];

int main() {
    RedBlackTree tree;

    srand(10);
    int x = rand();
    for (int i=1;i<=10000;i++) {
        while (f[x])
            x = rand() % 20000;
        f[x] = true;
        tree.insert(x);
    }

    std::cout << "Inorder traversal of the constructed tree: \n";
    tree.inorder();
    std::cout << std::endl << std::endl;


    std::cout << "tot_unbalance:" << tree.tot_unbalance << std::endl;
    std::cout << "tot_rotate:" << tree.tot_rotate << std::endl;
    std::cout << "tot_color:" << tree.tot_color << std::endl;


    return 0;
}