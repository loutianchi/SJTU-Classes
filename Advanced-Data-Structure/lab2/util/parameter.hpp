#pragma once
#include <cmath>

namespace HNSWLab {
    const int M = 30;
    const int M_max = 30;
    const int ef_construction = 100;
    const int ef_search = 100;
    const double mult_ = 1 / log(1.0 * M);
    const int threadNum = 8;
}