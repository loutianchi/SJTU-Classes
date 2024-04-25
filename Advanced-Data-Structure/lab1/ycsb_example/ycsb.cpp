#include <iostream>
#include <set>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>
#include "util.hpp"

void printLatency(std::vector<std::chrono::nanoseconds>& lat) {
  std::sort(lat.begin(), lat.end());
  std::cout << "Average latency: " << std::chrono::duration_cast<std::chrono::nanoseconds>(std::accumulate(lat.begin(), lat.end(), std::chrono::nanoseconds(0)) / lat.size()).count() << "ns" << std::endl;
  std::cout << "P50 latency: " << lat[lat.size() * 0.5].count() << "ns" << std::endl;
  std::cout << "P90 latency: " << lat[lat.size() * 0.9].count() << "ns" << std::endl;
  std::cout << "P99 latency: " << lat[lat.size() * 0.99].count() << "ns" << std::endl;
}

int main() {
  std::set<int32_t> s;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int32_t> dis(0, INT_MAX);
  std::vector<std::chrono::nanoseconds> insert_lat;
  std::vector<std::chrono::nanoseconds> find_lat;
  for (int i = 0; i < 1000; i++) {
    s.insert(dis(gen));
  }
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + std::chrono::seconds(10);
  std::uniform_int_distribution<int32_t> dis_zipf(0, INT_MAX);
  while (std::chrono::high_resolution_clock::now() < end) {
    auto op_start = std::chrono::high_resolution_clock::now();
    if (dis(gen) % 2 == 0) {
      s.insert(zipf());
      insert_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
    } else {
      s.find(zipf());
      find_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
    }
  }
  printLatency(insert_lat);
  printLatency(find_lat);
  return 0;
}
