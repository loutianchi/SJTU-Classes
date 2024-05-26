#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <future>

/*
std::atomic<int> currentThreads(0);  // 记录当前线程数

unsigned long long fibonacci(int n, int maxThreads);

// 辅助函数，用于在限制线程数的情况下计算斐波那契数列
unsigned long long fibonacci_thread(int n, int maxThreads) {
    if (n <= 1) {
        return n;
    }

    unsigned long long result1, result2;

    bool createThread = false;
    {
        // 控制线程数
        int expected = currentThreads.load();
        while (expected < maxThreads) {
            if (currentThreads.compare_exchange_weak(expected, expected + 1)) {
                createThread = true;
                break;
            }
        }
    }

    if (createThread) {
        std::thread t1([&result1, n, maxThreads]() { result1 = fibonacci(n - 1, maxThreads); });
        result2 = fibonacci(n - 2, maxThreads);
        t1.join();
        currentThreads--;
    } else {
        result1 = fibonacci(n - 1, maxThreads);
        result2 = fibonacci(n - 2, maxThreads);
    }

    return result1 + result2;
}

// 核心递归函数
unsigned long long fibonacci(int n, int maxThreads) {
    if (n <= 1) {
        return n;
    }

    unsigned long long result1, result2;

    bool createThread = false;
    {
        // 控制线程数
        int expected = currentThreads.load();
        while (expected < maxThreads) {
            if (currentThreads.compare_exchange_weak(expected, expected + 1)) {
                createThread = true;
                break;
            }
        }
    }

    if (createThread) {
        std::thread t1([&result1, n, maxThreads]() { result1 = fibonacci(n - 1, maxThreads); });
        result2 = fibonacci(n - 2, maxThreads);
        t1.join();
        currentThreads--;
    } else {
        result1 = fibonacci(n - 1, maxThreads);
        result2 = fibonacci(n - 2, maxThreads);
    }

    return result1 + result2;
}*/
unsigned long long fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

unsigned long long fibonacci_task(int n, int maxThreads, std::atomic<int>& currentThreads) {
    if (n <= 1) {
        return n;
    }

    if (currentThreads < maxThreads) {
        currentThreads++;
        std::future<unsigned long long> f1 = std::async(std::launch::async, fibonacci_task, n - 1, maxThreads, std::ref(currentThreads));
        unsigned long long result2 = fibonacci_task(n - 2, maxThreads, currentThreads);
        currentThreads--;
        return f1.get() + result2;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}



int main() {
    int n = 40;
    int maxThreads = 16;
    double ans = 0, sum = 0;
    for (int i = 1; i<= 100; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::atomic<int> currentThreads(0);
        unsigned long long result = fibonacci_task(n, maxThreads, currentThreads);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;  // 计算时间差
        std::cout << i << ":" << "Fib(" << n << ") = " << result << std::endl;
        sum += elapsed.count();
    }
    std::cout << sum / 100 << std::endl;
    return 0;
}
