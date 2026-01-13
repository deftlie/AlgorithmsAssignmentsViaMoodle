// Testing execution time of algorithms and saving results to CSV with full precision
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <limits> // for max_digits10

using namespace std;
using namespace chrono;

// Bubble sort
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// Insertion sort
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

// Merge sort helper
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> L(arr.begin() + left, arr.begin() + mid + 1);
    vector<int> R(arr.begin() + mid + 1, arr.begin() + right + 1);
    int i = 0, j = 0, k = left;
    while (i < L.size() && j < R.size())
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < L.size()) arr[k++] = L[i++];
    while (j < R.size()) arr[k++] = R[j++];
}

// Merge sort
void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Quick sort helper
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
        if (arr[j] < pivot)
            swap(arr[++i], arr[j]);
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Quick sort
void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

// Generate random array
vector<int> generateArray(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 10000;
    return arr;
}

// Measure execution time in seconds
double measure(function<void(vector<int>&)> sortFunc, vector<int> arr) {
    auto start = high_resolution_clock::now();
    sortFunc(arr);
    auto end = high_resolution_clock::now();
    return chrono::duration_cast<chrono::duration<double>>(end - start).count();
}

int main() {
    srand(time(nullptr));

    vector<int> sizes = {1, 5, 10, 25, 50, 100, 300, 500, 1000, 2000};
    const int runs = 5;

    // Open CSV file
    ofstream csv("sorting_results.csv");
    csv << "ArraySize,Algorithm,TimeSeconds\n";

    for (int n : sizes) {
        double bubbleSum = 0, insertionSum = 0, mergeSum = 0, quickSum = 0;

        for (int i = 0; i < runs; i++) {
            vector<int> base = generateArray(n);

            bubbleSum += measure(bubbleSort, base);
            insertionSum += measure(insertionSort, base);
            mergeSum += measure([&](vector<int>& v) { if(!v.empty()) mergeSort(v,0,v.size()-1); }, base);
            quickSum += measure([&](vector<int>& v) { if(!v.empty()) quickSort(v,0,v.size()-1); }, base);
        }

        double bubbleAvg = bubbleSum / runs;
        double insertionAvg = insertionSum / runs;
        double mergeAvg = mergeSum / runs;
        double quickAvg = quickSum / runs;

        // Save to CSV with full precision
        csv << n << ",Bubble," << scientific << setprecision(numeric_limits<double>::max_digits10) << bubbleAvg << "\n";
        csv << n << ",Insertion," << scientific << setprecision(numeric_limits<double>::max_digits10) << insertionAvg << "\n";
        csv << n << ",Merge," << scientific << setprecision(numeric_limits<double>::max_digits10) << mergeAvg << "\n";
        csv << n << ",Quick," << scientific << setprecision(numeric_limits<double>::max_digits10) << quickAvg << "\n";
    }

    csv.close();
    cout << "CSV file 'sorting_results.csv' created successfully!\n";

    return 0;
}
