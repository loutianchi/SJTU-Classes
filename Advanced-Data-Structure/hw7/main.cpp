#include <iostream>
#include "correctness.h"
#include "ycsb_test.h"
#include "select.h"

#define N1 100
#define N2 10000
#define N3 1000000

using namespace std;

int main()
{
    cout << "TEST1:" << endl;
    correctness test1;
    test1.initializer(N1);
//    test1.check("Test1, N = 100");
    ycsb_test::test(test1.a);

    cout << "TEST2:" << endl;
    correctness test2;
    test2.initializer(N2);
//    test2.check("Test2, N = 10000");
    ycsb_test::test(test2.a);

    cout << "TEST3:" << endl;
    correctness test3;
    test3.initializer(N3);
//    cout  << "Test3, N = 1000000 check done. No problem." << endl;
    ycsb_test::test(test3.a);

    return 0;
}
