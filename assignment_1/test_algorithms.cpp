// Testing the execution time of algorithms depending on array sizes
#include <iostream>
#include <vector>
#include <chrono>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

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
vector<int> generateArray(int n){
    vector<int> arr(n);
    for(int i=0;i<n;i++) arr[i]=rand()%10000;
    return arr;
}

// Measure execution time
double measure(function<void(vector<int>&)> f, vector<int> arr){
    auto start=high_resolution_clock::now();
    f(arr);
    auto end=high_resolution_clock::now();
    return chrono::duration_cast<chrono::duration<double>>(end-start).count();
}

//main
int main(){
    srand(time(nullptr));

    vector<int> sizes={1,5,10,25,50,100,300,500,1000,2000};
    const int runs=5;

    ofstream csv("sorting_results.csv");
    csv << "n,Bubble,Insertion,Merge,Quick\n";

    // fixed number of digits
    const int precision = 17;

    for(int n : sizes){
        double b_sum=0,i_sum=0,m_sum=0,q_sum=0;

        for(int r=0;r<runs;r++){
            vector<int> base=generateArray(n);

            b_sum += measure(bubbleSort, base);
            i_sum += measure(insertionSort, base);
            m_sum += measure([&](vector<int>& v){ if(!v.empty()) mergeSort(v,0,v.size()-1); }, base);
            q_sum += measure([&](vector<int>& v){ if(!v.empty()) quickSort(v,0,v.size()-1); }, base);
        }

        double b_avg=b_sum/runs;
        double i_avg=i_sum/runs;
        double m_avg=m_sum/runs;
        double q_avg=q_sum/runs;

        // write row to CSV
        csv << n << ","
            << scientific << setprecision(precision) << b_avg << ","
            << scientific << setprecision(precision) << i_avg << ","
            << scientific << setprecision(precision) << m_avg << ","
            << scientific << setprecision(precision) << q_avg << "\n";

        // print
        cout << "Array size: " << n << endl;
        cout << "Bubble:    " << scientific << setprecision(precision) << b_avg << " s" << endl;
        cout << "Insertion: " << scientific << setprecision(precision) << i_avg << " s" << endl;
        cout << "Merge:     " << scientific << setprecision(precision) << m_avg << " s" << endl;
        cout << "Quick:     " << scientific << setprecision(precision) << q_avg << " s" << endl;
        cout << "---------------------------------" << endl;
    }

    csv.close();
    cout << "CSV file 'sorting_results.csv' created successfully!" << endl;

    return 0;
}
