//
// Created by LEGION on 2024/4/30.
//

#include "select.h"
#include <algorithm>

#define Q 5

using namespace std;

int select::quickSelect(vector<int> &A, int k) {
    int n = A.size();
    k--;
    for (int low = 0, high = n - 1; low < high;) {
        int i = low, j = high, pivot = A[low + rand() % (high - low + 1)];
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

// Divide to L E G
pair<int,int> partition(vector<int>& arr, int left, int right, int pivot) {
    int pivotValue = arr[pivot];
    // like in qsort
    swap(arr[pivot], arr[right]);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
        if (arr[i] < pivotValue) {
            swap(arr[storeIndex], arr[i]);
            storeIndex++;
        }
    }
    swap(arr[storeIndex], arr[right]);
    int L = 0, E = 0;
    for (int i = left; i < right; i++) {
        if (arr[i] < pivotValue) L++;
        else if (arr[i] == pivotValue) E++;
        else break;
    }
    return make_pair(L, E);
}

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
