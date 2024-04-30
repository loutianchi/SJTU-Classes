//
// Created by LEGION on 2024/4/30.
//

#ifndef HW7_CORRECTNESS_H
#define HW7_CORRECTNESS_H

#include <string>
#include "select.h"

class correctness {
    std::vector<int> b;
public:
    std::vector<int> a;
    int N;
    int initializer(int n);
    int check(std::string name);
};


#endif //HW7_CORRECTNESS_H
