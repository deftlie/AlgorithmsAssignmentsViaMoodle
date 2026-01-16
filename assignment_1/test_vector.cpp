// C++ Vector test
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

class MyVector {
private:
    int* data;
    size_t sz;
    size_t cap;

    void reallocate(size_t newCap) {
        int* newData = new int[newCap];
        for (size_t i = 0; i < sz; ++i)
            newData[i] = data[i];

        delete[] data;
        data = newData;
        cap = newCap;

        cout << "[MyVector] realloc to " << cap << "\n";
    }

public:
    MyVector() : data(nullptr), sz(0), cap(0) {}
    ~MyVector() { delete[] data; }

    void push_back(int value) {
        if (sz == cap) {
            size_t newCap = (cap == 0) ? 1 : cap * 2;
            reallocate(newCap);
        }
        data[sz++] = value;
    }
};

class LinkedList {
private:
    struct Node {
        int value;
        Node* next;
        Node(int v) : value(v), next(nullptr) {}
    };

    Node* head;
    Node* tail;

public:
    LinkedList() : head(nullptr), tail(nullptr) {}

    ~LinkedList() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }

    void push_back(int value) {
        Node* n = new Node(value);
        if (!head) {
            head = tail = n;
        } else {
            tail->next = n;
            tail = n;
        }
    }
};

// Benchmarks
void test_myvector(size_t N) {
    MyVector v;
    auto start = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; ++i)
        v.push_back(i);

    auto end = chrono::high_resolution_clock::now();
    cout << "MyVector time: "
         << chrono::duration<double>(end - start).count()
         << " s\n\n";
}

void test_stdvector(size_t N) {
    vector<int> v;
    size_t lastCap = v.capacity();

    auto start = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; ++i) {
        v.push_back(i);
        if (v.capacity() != lastCap) {
            cout << "[std::vector] realloc to "
                 << v.capacity() << "\n";
            lastCap = v.capacity();
        }
    }

    auto end = chrono::high_resolution_clock::now();
    cout << "std::vector time: "
         << chrono::duration<double>(end - start).count()
         << " s\n\n";
}

void test_linkedlist(size_t N) {
    LinkedList list;
    auto start = chrono::high_resolution_clock::now();

    for (size_t i = 0; i < N; ++i)
        list.push_back(i);

    auto end = chrono::high_resolution_clock::now();
    cout << "LinkedList time: "
         << chrono::duration<double>(end - start).count()
         << " s\n\n";
}

int main() {
    const size_t N = 5'000'000;

    cout << "=== Testing MyVector ===\n";
    test_myvector(N);

    cout << "=== Testing std::vector ===\n";
    test_stdvector(N);

    cout << "=== Testing LinkedList ===\n";
    test_linkedlist(N);

    return 0;
}
