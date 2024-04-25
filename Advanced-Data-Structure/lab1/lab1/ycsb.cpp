#include <iostream>
#include <set>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>
#include "util.hpp"
#include <climits>
#include <string>

#include "radix_tree.hpp"
#include "compressed_radix_tree.hpp"
#include "red_black_tree.hpp"

#define workload1 1
#define workload2 2
#define workload3 3

void printLatency(std::vector<std::chrono::nanoseconds>& lat)
{
    if (lat.size() == 0) {
        std::cout << "      This operation not exist" << std::endl;
        return;
    }
    std::sort(lat.begin(), lat.end());
    std::cout << "      Average latency: " << std::chrono::duration_cast<std::chrono::nanoseconds>(std::accumulate(lat.begin(), lat.end(), std::chrono::nanoseconds(0)) / lat.size()).count() << "ns" << std::endl;
    std::cout << "      P50 latency: " << lat[lat.size() * 0.5].count() << "ns" << std::endl;
    std::cout << "      P90 latency: " << lat[lat.size() * 0.9].count() << "ns" << std::endl;
    std::cout << "      P99 latency: " << lat[lat.size() * 0.99].count() << "ns" << std::endl;
}

template <typename TreeType>
void testType(const std::string name, int workloadType)
{
    TreeType *s = new TreeType();
    std::cout << "Type: " << name << "  Workload: " << workloadType << std::endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dis(0, INT_MAX);
    std::vector<std::chrono::nanoseconds> insert_lat;
    std::vector<std::chrono::nanoseconds> find_lat;
    std::vector<std::chrono::nanoseconds> remove_lat;
    for (int i = 0; i < 1000; i++) {
        s->insert(dis(gen));
    }
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::seconds(10);
    std::uniform_int_distribution<int32_t> dis_zipf(0, INT_MAX);

    int genRes;

    while (std::chrono::high_resolution_clock::now() < end) {
        auto op_start = std::chrono::high_resolution_clock::now();
        genRes = dis(gen);
        switch (workloadType)
        {
        case workload1:
            if (genRes % 2 == 0) {
                s->insert(zipf());
                insert_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            } else {
                s->find(zipf());
                find_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            }
            break;
        case workload2:
            s->find(zipf());
            find_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            break;
        case workload3:
            if (genRes % 4 == 1) {
                s->insert(zipf());
                insert_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            }
            else if (genRes % 2 == 0) {
                s->find(zipf());
                find_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            }
            else {
                s->remove(zipf());
                remove_lat.push_back(std::chrono::high_resolution_clock::now() - op_start);
            }
            break;
       
        default:
            break;
        }
    }
    std::cout << "    Insert latency: " << std::endl;
    printLatency(insert_lat);
    std::cout << "    Find latency: " << std::endl;
    printLatency(find_lat);
    std::cout << "    Remove latency: " << std::endl;
    printLatency(remove_lat);
    delete s;
}

int main()
{
    testType<class RadixTree>("Radix Tree", workload1);
    testType<class RadixTree>("Radix Tree", workload2);
    testType<class RadixTree>("Radix Tree", workload3);
    
    testType<class CompressedRadixTree>("Compressed Radix Tree", workload1);
    testType<class CompressedRadixTree>("Compressed Radix Tree", workload2);
    testType<class CompressedRadixTree>("Compressed Radix Tree", workload3);

    testType< Set<int32_t> >("Red-Black Tree", workload1);
    testType< Set<int32_t> >("Red-Black Tree", workload2);
    testType< Set<int32_t> >("Red-Black Tree", workload3);

    return 0;
}
