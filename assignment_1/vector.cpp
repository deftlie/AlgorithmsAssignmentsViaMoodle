//C++ Vector implementation
#include <iostream>
#include <stdexcept>

class MyVector {
private:
    int* data;
    size_t sz;
    size_t cap;

    void reallocate(size_t newCapacity) {
        int* newData = new int[newCapacity];
        for (size_t i = 0; i < sz; ++i)
            newData[i] = data[i];

        delete[] data;
        data = newData;
        cap = newCapacity;
    }

public:
    MyVector() : data(nullptr), sz(0), cap(0) {}

    ~MyVector() {
        delete[] data;
    }

    size_t size() const {
        return sz;
    }

    size_t capacity() const {
        return cap;
    }

    int& operator[](size_t index) {
        return data[index];
    }

    int get(size_t index) const {
        if (index >= sz)
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    void set(size_t index, int value) {
        if (index >= sz)
            throw std::out_of_range("Index out of range");
        data[index] = value;
    }

    void push_back(int value) {
        if (sz == cap) {
            size_t newCapacity = (cap == 0) ? 1 : cap * 2;
            reallocate(newCapacity);
        }
        data[sz++] = value;
    }

    void resize(size_t newSize) {
        if (newSize > cap)
            reallocate(newSize);
        sz = newSize;
    }

    void erase(size_t index) {
        if (index >= sz)
            throw std::out_of_range("Index out of range");

        for (size_t i = index; i + 1 < sz; ++i)
            data[i] = data[i + 1];
        --sz;
    }

    void erase(size_t l, size_t r) {
        if (l >= sz || r > sz || l >= r)
            throw std::out_of_range("Invalid range");

        size_t count = r - l;
        for (size_t i = l; i + count < sz; ++i)
            data[i] = data[i + count];
        sz -= count;
    }
};

//main
int main() {
    MyVector v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    v.erase(1); // удаляем 20

    std::cout << v[0] << " " << v[1] << std::endl;
    return 0;
}
