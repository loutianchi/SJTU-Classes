//
// Created by LEGION on 2024/4/30.
//

#include "correctness.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cassert>

#define T 1000

using namespace std;

int correctness::initializer(int n)
{
    a.clear();
    int x;
    srand(time(nullptr));
    for (int i = 0; i < n; ++i) {
        x = n - i;
        a.push_back(x);
        b.push_back(x);
    }
    sort(b.begin(), b.end());
    N = n;
}

int correctness::check(string name)
{
    for (int i = 0; i < T; i++) {
        int x = rand() % N;
        assert(select::linearSelect(a, x) == b[x]);
        assert(select::quickSelect(a, x + 1) == b[x]);
    }
    cout << name << " check done. No problem." << endl;
}