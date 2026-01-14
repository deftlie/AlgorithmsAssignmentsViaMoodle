#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <chrono>
#include <random>
#include <numeric>
#include <functional>

using namespace std;
using namespace chrono;

// ==================== BINARY TREE ====================
struct BinaryNode {
    int key;
    BinaryNode* left;
    BinaryNode* right;
    BinaryNode(int k) : key(k), left(nullptr), right(nullptr) {}
};

class BinaryTree {
public:
    BinaryNode* root = nullptr;
    void insert(int key) { root = insertRec(root, key); }
    void remove(int key) { root = removeRec(root, key); }
    int height() { return heightRec(root); }

private:
    BinaryNode* insertRec(BinaryNode* node, int key) {
        if (!node) return new BinaryNode(key);
        if (key < node->key) node->left = insertRec(node->left, key);
        else node->right = insertRec(node->right, key);
        return node;
    }

    BinaryNode* removeRec(BinaryNode* node, int key) {
        if (!node) return nullptr;
        if (key < node->key) node->left = removeRec(node->left, key);
        else if (key > node->key) node->right = removeRec(node->right, key);
        else {
            if (!node->left) return node->right;
            if (!node->right) return node->left;
            BinaryNode* minNode = node->right;
            while (minNode->left) minNode = minNode->left;
            node->key = minNode->key;
            node->right = removeRec(node->right, minNode->key);
        }
        return node;
    }

    int heightRec(BinaryNode* node) {
        if (!node) return 0;
        return 1 + max(heightRec(node->left), heightRec(node->right));
    }
};

// ==================== TERNARY TREE ====================
struct TernaryNode {
    int key1, key2;
    TernaryNode* left;
    TernaryNode* middle;
    TernaryNode* right;
    TernaryNode(int k) : key1(k), key2(-1), left(nullptr), middle(nullptr), right(nullptr) {}
};

class TernaryTree {
public:
    TernaryNode* root = nullptr;
    void insert(int key) { root = insertRec(root, key); }
    int height() { return heightRec(root); }

private:
    TernaryNode* insertRec(TernaryNode* node, int key) {
        if (!node) return new TernaryNode(key);
        if (node->key2 == -1) {
            if (key < node->key1) { node->key2 = node->key1; node->key1 = key; }
            else node->key2 = key;
            return node;
        }
        if (key < node->key1) node->left = insertRec(node->left, key);
        else if (key < node->key2) node->middle = insertRec(node->middle, key);
        else node->right = insertRec(node->right, key);
        return node;
    }

    int heightRec(TernaryNode* node) {
        if (!node) return 0;
        return 1 + max({heightRec(node->left), heightRec(node->middle), heightRec(node->right)});
    }
};

// ==================== AVL TREE ====================
struct AVLNode {
    int key;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    AVLNode* root = nullptr;
    void insert(int key) { root = insertRec(root, key); }
    int height() { return root ? root->height : 0; }

private:
    int getHeight(AVLNode* node) { return node ? node->height : 0; }
    int getBalance(AVLNode* node) { return node ? getHeight(node->left) - getHeight(node->right) : 0; }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        return y;
    }

    AVLNode* insertRec(AVLNode* node, int key) {
        if (!node) return new AVLNode(key);
        if (key < node->key) node->left = insertRec(node->left, key);
        else node->right = insertRec(node->right, key);

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && key < node->left->key) return rightRotate(node);
        if (balance < -1 && key > node->right->key) return leftRotate(node);
        if (balance > 1 && key > node->left->key) { node->left = leftRotate(node->left); return rightRotate(node); }
        if (balance < -1 && key < node->right->key) { node->right = rightRotate(node->right); return leftRotate(node); }

        return node;
    }
};

// ==================== HELPERS ====================
vector<int> generateRandomKeys(int n) {
    vector<int> keys(n);
    iota(keys.begin(), keys.end(), 1);
    random_device rd;
    mt19937 g(rd());
    shuffle(keys.begin(), keys.end(), g);
    return keys;
}

vector<int> buildPerfectOrder(vector<int> keys) {
    sort(keys.begin(), keys.end());
    vector<int> order;
    auto dfs = [&](auto&& self, int l, int r) -> void {
        if (l > r) return;
        int mid = (l + r) / 2;
        order.push_back(keys[mid]);
        self(self, l, mid - 1);
        self(self, mid + 1, r);
    };
    dfs(dfs, 0, keys.size() - 1);
    return order;
}

// Функция для медианы
long long median(vector<long long> v) {
    sort(v.begin(), v.end());
    return v[v.size()/2];
}

// Функция для измерения медианного времени вставки
template<typename TreeType>
pair<long long,int> measureInsertMedian(const vector<int>& keys, int runs) {
    vector<long long> times;
    int h = 0;
    for(int i=0;i<runs;i++){
        TreeType tree;
        auto start = high_resolution_clock::now();
        for(int k: keys) tree.insert(k);
        auto end = high_resolution_clock::now();
        times.push_back(duration_cast<microseconds>(end-start).count());
        h = tree.height();
    }
    return {median(times), h};
}

// ==================== MAIN ====================
int main() {
    int n = 255; // увеличено для наглядного эффекта
    auto keys = generateRandomKeys(n);
    auto perfectOrder = buildPerfectOrder(keys);
    int runs = 5;

    // --- BinaryTree ---
    auto [btRandomTime, btRandomHeight] = measureInsertMedian<BinaryTree>(keys, runs);
    auto [btBestTime, btBestHeight] = measureInsertMedian<BinaryTree>(perfectOrder, runs);
    cout << "BinaryTree Random Insert: " << btRandomTime << " us, Height: " << btRandomHeight << "\n";
    cout << "BinaryTree Best Insert: " << btBestTime << " us, Height: " << btBestHeight << "\n";

    // --- std::set ---
    vector<long long> setRandomTimes;
    for(int i=0;i<runs;i++){
        set<int> s;
        auto start = high_resolution_clock::now();
        for(int k: keys) s.insert(k);
        auto end = high_resolution_clock::now();
        setRandomTimes.push_back(duration_cast<microseconds>(end-start).count());
    }
    cout << "std::set Random Insert (median): " << median(setRandomTimes) << " us\n";

    // --- TernaryTree ---
    auto [ttRandomTime, ttRandomHeight] = measureInsertMedian<TernaryTree>(keys, runs);
    auto [ttBestTime, ttBestHeight] = measureInsertMedian<TernaryTree>(perfectOrder, runs);
    cout << "TernaryTree Random Insert: " << ttRandomTime << " us, Height: " << ttRandomHeight << "\n";
    cout << "TernaryTree Best Insert: " << ttBestTime << " us, Height: " << ttBestHeight << "\n";

    // --- AVLTree ---
    auto [avlRandomTime, avlRandomHeight] = measureInsertMedian<AVLTree>(keys, runs);
    auto [avlBestTime, avlBestHeight] = measureInsertMedian<AVLTree>(perfectOrder, runs);
    cout << "AVLTree Random Insert: " << avlRandomTime << " us, Height: " << avlRandomHeight << "\n";
    cout << "AVLTree Best Insert: " << avlBestTime << " us, Height: " << avlBestHeight << "\n";

    return 0;
}
