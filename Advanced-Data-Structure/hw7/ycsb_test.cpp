//
// Created by LEGION on 2024/4/30.
//

#include "ycsb_test.h"
#include <cstdlib>

void ycsb_test::test(std::vector<int> &A)
{
  std::vector<std::chrono::nanoseconds> quickSelect_lat;
  std::vector<std::chrono::nanoseconds> linearSelect_lat;

  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + std::chrono::seconds(10);

  int n = A.size();
  std::vector<int> B;

  while (std::chrono::high_resolution_clock::now() < end) {
    B.clear(); B = A;
    auto op_start = std::chrono::high_resolution_clock::now();
    select::quickSelect(B, std::rand() % n+1);
    quickSelect_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
  }
  std::cout << "Select function:" << "Optimized Quick Select" << std::endl;
  printLatency(quickSelect_lat);

  start = std::chrono::high_resolution_clock::now();
  end = start + std::chrono::seconds(10);
  while (std::chrono::high_resolution_clock::now() < end) {
    B.clear(); B = A;
    auto op_start = std::chrono::high_resolution_clock::now();
    select::linearSelect(B, std::rand() % n);
    linearSelect_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
  }
  std::cout << "Select function:" << "Linear Select" << std::endl;
  printLatency(linearSelect_lat);
}