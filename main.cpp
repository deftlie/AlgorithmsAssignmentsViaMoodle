#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;
using namespace chrono;

// ---------------- Bubble Sort ----------------
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// ---------------- Insertion Sort ----------------
void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---------------- Merge Sort ----------------
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> L(arr.begin() + left, arr.begin() + mid + 1);
    vector<int> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;

    while (i < L.size() && j < R.size())
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];

    while (i < L.size()) arr[k++] = L[i++];
    while (j < R.size()) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// ---------------- Quick Sort ----------------
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++)
        if (arr[j] < pivot)
            swap(arr[++i], arr[j]);

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

// ---------------- Utils ----------------

// Generate random array
vector<int> generateArray(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 10000;
    return arr;
}

// Measure execution time (seconds)
double measure(function<void(vector<int>&)> sortFunc, vector<int> arr) {
    auto start = high_resolution_clock::now();
    sortFunc(arr);
    auto end = high_resolution_clock::now();
    return duration_cast<duration<double>>(end - start).count();
}

// ---------------- Main ----------------
int main() {
    srand(time(nullptr));

    vector<int> sizes = {1, 5, 10, 25, 50, 100, 300, 500, 1000, 2000};
    const int runs = 5;

    cout << fixed << scientific;

    for (int n : sizes) {
        double bubbleSum = 0;
        double insertionSum = 0;
        double mergeSum = 0;
        double quickSum = 0;

        for (int i = 0; i < runs; i++) {
            vector<int> base = generateArray(n);

            bubbleSum += measure(bubbleSort, base);
            insertionSum += measure(insertionSort, base);

            mergeSum += measure([&](vector<int>& v) {
                if (!v.empty())
                    mergeSort(v, 0, v.size() - 1);
            }, base);

            quickSum += measure([&](vector<int>& v) {
                if (!v.empty())
                    quickSort(v, 0, v.size() - 1);
            }, base);
        }

        cout << "Array size: " << n << endl;
        cout << "Bubble:    " << bubbleSum / runs << " s" << endl;
        cout << "Insertion: " << insertionSum / runs << " s" << endl;
        cout << "Merge:     " << mergeSum / runs << " s" << endl;
        cout << "Quick:     " << quickSum / runs << " s" << endl;
        cout << "---------------------------------" << endl;
    }

    return 0;
}
