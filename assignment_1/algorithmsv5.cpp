//Additional task for grade 5
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

long long pivotCount = 0;

// Partition with pivot counting
int partition(vector<int>& arr, int low, int high) {
    pivotCount++;                 // считаем pivot
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Quick Sort
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
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1'000'000);

    for (int& x : arr)
        x = dist(gen);

    return arr;
}

int main() {
    vector<int> sizes = {1000, 5000, 10000, 20000, 50000};

    cout << "Size\tPivots\tTime (s)\n";

    for (int n : sizes) {
        vector<int> arr = generateArray(n);

        pivotCount = 0;
        auto start = chrono::high_resolution_clock::now();

        quickSort(arr, 0, n - 1);

        auto end = chrono::high_resolution_clock::now();
        double time = chrono::duration<double>(end - start).count();

        cout << n << "\t" << pivotCount << "\t" << time << endl;
    }

    return 0;
}
