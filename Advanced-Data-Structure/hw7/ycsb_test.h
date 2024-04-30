//
// Created by LEGION on 2024/4/30.
//

#ifndef HW7_YCSB_TEST_H
#define HW7_YCSB_TEST_H

#include <chrono>
#include "select.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

class ycsb_test {
    static void printLatency(std::vector<std::chrono::nanoseconds>& lat) {
      std::sort(lat.begin(), lat.end());
      std::cout << "    Average latency: " << std::chrono::duration_cast<std::chrono::nanoseconds>(std::accumulate(lat.begin(), lat.end(), std::chrono::nanoseconds(0)) / lat.size()).count() << "ns" << std::endl;
      std::cout << "    P50 latency: " << lat[lat.size() * 0.5].count() << "ns" << std::endl;
      std::cout << "    P90 latency: " << lat[lat.size() * 0.9].count() << "ns" << std::endl;
      std::cout << "    P99 latency: " << lat[lat.size() * 0.99].count() << "ns" << std::endl;
    }
public:
    static void test(std::vector<int> &A);
};

#endif //HW7_YCSB_TEST_H
