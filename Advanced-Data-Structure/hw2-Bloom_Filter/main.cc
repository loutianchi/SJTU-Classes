#include <iostream>
#include "MurmurHash3.h"

#include <map>
#include <cstring>
#include <iomanip>

int main() {
    std::map<uint64_t, uint64_t> mp;
    uint64_t key;
    uint64_t hash[2] = {0};
    
    int mDn ,k ,m;
    std::cin >> mDn >> k;
    int totN = 0, totW = 0;

    std::cout << std::fixed << std::setprecision(5);

    for (int n = 50; n < 10000; n += 50) {
        m = n * mDn;
        totN += n;

        bool *flag;
        flag = new bool[m];
        for (int i = 0; i < m; i++)
            flag[i] = false;

        for (int i = 0; i < n; i++) {
            key = i;
            for (int j = 1; j <= k; j++) {
                MurmurHash3_x64_128(&key, sizeof(key), j, hash);
                flag[hash[0] % m] = true;
            }
        }
            
        bool isExi;
        int totWrong = 0;
        for (int i = n + 1; i <= n * 2; i++) {
            key = i;
            isExi = true;
            for (int j = 1; j <= k; j++) {
                MurmurHash3_x64_128(&key, sizeof(key), j, hash);
                if (!flag[hash[1] % m]) isExi = false;
            }
            if (isExi)
                totWrong++;
        }
        totW += totWrong;
        //std::cout << totWrong * 1.0 / n << std::endl;

        delete []flag;
    }
    std::cout << std::endl << totW << " " << totN << std::endl;
    std::cout << std::endl << totW * 1.0 / totN << std::endl;

    return 0;
}
