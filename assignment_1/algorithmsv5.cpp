#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;

long long pivotCount = 0;

// ======================
// Single-pivot Quick Sort
// ======================
int partitionSingle(vector<int>& arr, int low, int high) {
    pivotCount++;
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
        if (arr[j] < pivot)
            swap(arr[++i], arr[j]);
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortSingle(vector<int>& arr, int low, int high) {
    if (low < high) {
        int p = partitionSingle(arr, low, high);
        quickSortSingle(arr, low, p - 1);
        quickSortSingle(arr, p + 1, high);
    }
}

// ======================
// Dual-pivot Quick Sort
// ======================
void quickSortDual(vector<int>& arr, int low, int high) {
    if (low >= high) return;
    pivotCount += 2;
    if (arr[low] > arr[high]) swap(arr[low], arr[high]);
    int pivot1 = arr[low];
    int pivot2 = arr[high];

    int lt = low + 1, gt = high - 1, i = lt;
    while (i <= gt) {
        if (arr[i] < pivot1) swap(arr[i++], arr[lt++]);
        else if (arr[i] > pivot2) swap(arr[i], arr[gt--]);
        else i++;
    }
    swap(arr[low], arr[--lt]);
    swap(arr[high], arr[++gt]);

    quickSortDual(arr, low, lt - 1);
    quickSortDual(arr, lt + 1, gt - 1);
    quickSortDual(arr, gt + 1, high);
}

// ======================
// Triple-pivot Quick Sort (учебная версия)
// ======================
void quickSortTriple(vector<int>& arr, int low, int high) {
    if (low >= high) return;
    pivotCount += 3;
    int mid = low + (high - low) / 2;
    vector<pair<int,int>> piv = {{arr[low], low}, {arr[mid], mid}, {arr[high], high}};
    sort(piv.begin(), piv.end());
    swap(arr[low], arr[piv[0].second]);
    swap(arr[mid], arr[piv[1].second]);
    swap(arr[high], arr[piv[2].second]);

    int p1 = arr[low], p2 = arr[mid], p3 = arr[high];
    vector<int> left, middle, right;
    for (int i = low + 1; i < high; i++) {
        if (arr[i] < p1) left.push_back(arr[i]);
        else if (arr[i] < p2) middle.push_back(arr[i]);
        else if (arr[i] < p3) right.push_back(arr[i]);
        else middle.push_back(arr[i]);
    }

    int idx = low;
    for (int x : left) arr[idx++] = x;
    arr[idx++] = p1;
    for (int x : middle) arr[idx++] = x;
    arr[idx++] = p2;
    for (int x : right) arr[idx++] = x;
    arr[idx++] = p3;

    if (!left.empty()) quickSortTriple(arr, low, low + left.size() - 1);
    if (!middle.empty()) quickSortTriple(arr, low + left.size() + 1, low + left.size() + middle.size() - 1);
    if (!right.empty()) quickSortTriple(arr, low + left.size() + middle.size() + 2, high - 1);
}

// ======================
// Generate random array
// ======================
vector<int> generateArray(int n) {
    vector<int> arr(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1'000'000);
    for (int& x : arr) x = dist(gen);
    return arr;
}

// ======================
// Benchmark wrappers
// ======================
double benchmarkSingle(vector<int> arr) {
    pivotCount = 0;
    auto start = chrono::high_resolution_clock::now();
    quickSortSingle(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

double benchmarkDual(vector<int> arr) {
    pivotCount = 0;
    auto start = chrono::high_resolution_clock::now();
    quickSortDual(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

double benchmarkTriple(vector<int> arr) {
    pivotCount = 0;
    auto start = chrono::high_resolution_clock::now();
    quickSortTriple(arr, 0, arr.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

// ======================
// Main
// ======================
int main() {
    vector<int> sizes = {1000,5000,10000,20000,30000,40000,50000,60000,70000,80000,90000,100000};

    // Заголовок таблицы
    cout << setw(8) << "Size"
         << setw(16) << "SingleTime"
         << setw(16) << "DualTime"
         << setw(16) << "TripleTime" << endl;

    for (int n : sizes) {
        vector<int> arr = generateArray(n);

        double t1 = benchmarkSingle(arr);
        double t2 = benchmarkDual(arr);
        double t3 = benchmarkTriple(arr);

        cout << setw(8) << n
             << setw(16) << fixed << setprecision(6) << t1
             << setw(16) << fixed << setprecision(6) << t2
             << setw(16) << fixed << setprecision(6) << t3 << endl;
    }

    return 0;
}
