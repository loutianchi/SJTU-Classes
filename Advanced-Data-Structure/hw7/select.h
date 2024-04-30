//
// Created by LEGION on 2024/4/30.
//

#ifndef HW7_SELECT_H
#define HW7_SELECT_H

#include <vector>

class select {
    static int medianOfMedians(std::vector<int>& arr, int left, int right, int q);
public:
    static int quickSelect(std::vector<int> &A, int k);
    static int linearSelect(std::vector<int>& arr, int k);
};


#endif //HW7_SELECT_H
