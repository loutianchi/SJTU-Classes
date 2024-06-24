#include <iostream>
#include <atomic>
#include <thread>
#include <cstring>

int a[4];
std::atomic<int> count(0);

void increase_count(int n,int id) {
    for (int i = 0; i < n; ++i) {
        a[id] ++;  // 原子增加操作
    }
    count += a[id];
}

void thread_print(int x) {
    std::cout << "Count: " << count << std::endl;
}

int main() {
        memset(a, 0 ,sizeof(a));
        count = 0;
        const int threads = 4;
        std::thread t[threads];

        for (int i = 0; i < threads; ++i) {
            t[i] = std::thread(increase_count, 100, i);
        }

        for (int i = 0; i < threads; ++i) {
            t[i].join();
        }

        std::thread ta = std::thread(thread_print, count.load());
        ta.join();

    return 0;
}