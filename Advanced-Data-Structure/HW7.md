# HW7

### 代码实现

##### Quick Select

每次做半边的排序，类似快排。

```cpp
int select::quickSelect(vector<int> &A, int k) {
    int n = A.size();
    k--;
    for (int low = 0, high = n - 1; low < high;) {
        int i = low, j = high, pivot = A[low];
        while (i < j) {
            while (i < j && pivot <= A[j]) --j;
            A[i] = A[j];
            while (i < j && pivot >= A[i]) ++i;
            A[j] = A[i];
        }
        A[i] = pivot;
        if (k <= i) high = i - 1;
        if (k >= i) low = i + 1;
    }
    return A[k];
}
```

##### Linear Select

分为三个子函数：

1. partition，负责将数组按照 小于pivot，等于pivot，大于pivot 进行排序，时间复杂度为O(n)。
2. medianOfMedians，负责按照 Q 进行划分，将每个部分的中位数插入到一个新的数组中。如果还是太长会调用后面的 linearSelect 函数继续寻找。
3. linearSelect，处理剩余数字不足 Q 的情况；并且更改每次需要寻找的边界（left，right）。

第一个函数较为简单，不再赘述。第二个、第三个函数如下：

```cpp
int select::medianOfMedians(vector<int>& arr, int left, int right, int q) {
    vector<int> medians;
    int numMedians = (right - left + 1) / q;
    for (int i = 0; i < numMedians; i++) {
        vector<int> temp(arr.begin() + left + i * q, arr.begin() + left + (i + 1) * q);
        sort(temp.begin(), temp.end());
        medians.push_back(temp[q / 2]);
    }

    // If last group has less than q elements, sort them and add their median
    int remainingElements = (right - left + 1) % q;
    if (remainingElements > 0) {
        vector<int> temp(arr.begin() + left + numMedians * q, arr.begin() + right + 1);
        sort(temp.begin(), temp.end());
        medians.push_back(temp[remainingElements / 2]);
    }

    // Only one median, return it
    if (medians.size() == 1) {
        return medians[0];
    }
    // More than one median, recursively find the median of the medians
    else
    {
        return linearSelect(medians, medians.size() / 2);
    }
}

// The linearSelect function
int select::linearSelect(vector<int>& arr, int k) {
    int left = 0;
    int right = arr.size() - 1;
    while (left <= right) {
        if (right - left + 1 <= Q) {
            sort(arr.begin() + left, arr.begin() + right + 1);
            return arr[left + k];
        }
        // Find the pivot element
        int pivot = medianOfMedians(arr, left, right, Q);
        int pivotIndex = find(arr.begin() + left, arr.begin() + right + 1, pivot) - arr.begin();
        int L, E;
        pair<int, int> res = partition(arr, left, right, pivotIndex);
        L = res.first, E = res.second;

        // Check the pivot element's position against k
        if (k < L) {
            right = left + L - 1;
        } else if (k < L + E) {
            return arr[left + k];
        } else {
            left = left + L + E;
            k -= L + E;
        }
    }
}
```

使用 correctness 进行测试，结果无误。

### 实验设计及结果分析

#### 测试配置

在 Clion 中进行测试，使用类似 ycsb 的方式进行测试。

每次运行 10s （但是至少完成运行一次），每次向同一个测试数据集类型（按数据规模及顺序分类）的数组，随机一个 序号k， 用两种方式分别选择第k小的数字，各自统计运行时间，输出平均耗时、P50、P90、P99耗时。

#### 数据规模：

分为 N = 100，N = 10 000， N = 1 000 000 三种数据规模进行实验，使用乱序的数据进行测试，结果如下。

| <img src="C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120001716.png" alt="image-20240430120001716" /> | ![image-20240430120510967](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120510967.png) | ![image-20240430120430782](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120430782.png) |
| :----------------------------------------------------------: | ------------------------------------------------------------ | :----------------------------------------------------------: |
|                           N = 100                            | N = 10 000                                                   |                        N = 1 000 000                         |

观察上面的实验结果，可以发现在这种实验配置下，两种算法的耗时都随规模增大而上升，上升的倍数基本与 N 值扩大的倍数相等，在 100 倍左右。对比 Quick Select 和 Linear Select，可以看出在这个工作负载中，Linear Select 的耗时约为 Quick Select 的 15~20 倍。这是因为在乱序的情况下，两中算法的平均时间复杂度均为 O(n)，但是由于 Linear Select 的操作更多，有更大的常数，因此体现出来的耗时就更长了。

#### 数据顺序

1. 顺序：

   | ![image-20240430141636513](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141636513.png) | ![image-20240430141654304](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141654304.png) | ![image-20240430141706702](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141706702.png) |
   | :----------------------------------------------------------: | ------------------------------------------------------------ | :----------------------------------------------------------: |
   |                           N = 100                            | N = 10 000                                                   |                        N = 1 000 000                         |

2. 倒序：

   | ![image-20240430141805119](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141805119.png) | ![image-20240430141826965](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141826965.png) | ![image-20240430141943627](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430141943627.png) |
   | :----------------------------------------------------------: | ------------------------------------------------------------ | :----------------------------------------------------------: |
   |                           N = 100                            | N = 10 000                                                   |                        N = 1 000 000                         |

3. 乱序：

   | <img src="C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120001716.png" alt="image-20240430120001716" /> | ![image-20240430120510967](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120510967.png) | ![image-20240430120430782](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430120430782.png) |
   | :----------------------------------------------------------: | ------------------------------------------------------------ | :----------------------------------------------------------: |
   |                           N = 100                            | N = 10 000                                                   |                        N = 1 000 000                         |

分析实验结果，可以发现，在顺序和倒序的情况下，Quick Select 的时延上升迅速，达到了 O(n^2)；而 Linear Select 的时延仍然保持线性不变，为 O(n)。在 N 较小（为100）时，时间复杂度的差异仍未影响到常数，但是在 N 增大后，影响就非常明显了（使用Quick Select在一个测试周期内甚至做不完一次查找），Quick Select执行一次选择接近一分钟，而 Linear Select 仍为毫秒级别。

此外，倒序的时延比顺序高，这可能是因为倒序需要多做一些交换的操作，而且每次都需要执行，导致了这种结果。

### 实验结论

虽然 Linear Select 在理论上的时间复杂度是优于 Quick Select，但是在数据乱序的情况下，时延上的表现是远远不如 Quick Select 的。因此在实际使用中，不仅要考虑理论的时间复杂度，也要考虑常数带来的影响。但是在数据集可能出现最坏情况（顺序、倒序）的情况下，使用 Linear Select 的稳定性就会比 Quick Select 高的多，因此也要考虑最后的输入情况来考虑使用哪种算法。

### 一些思考

在我看来，使用 Quick Select 并且在选择 pivot 时引入随机性，随机选择一个元素来决定如何划分是一个可能更优的方案。

进行实验验证的结果如下：

| ![image-20240430144114090](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430144114090.png) | ![image-20240430144138027](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430144138027.png) | ![image-20240430144155404](C:\Users\LEGION\AppData\Roaming\Typora\typora-user-images\image-20240430144155404.png) |
| :----------------------------------------------------------: | ------------------------------------------------------------ | :----------------------------------------------------------: |
|                           N = 100                            | N = 10 000                                                   |                        N = 1 000 000                         |

可以看出，经过优化的 Quick Select 的时间复杂度在倒序（在之前是最糟糕的情况）时，也能有很好的时间复杂度。