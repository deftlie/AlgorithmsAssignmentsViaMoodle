#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// ----------------- Struct -----------------
struct CardRow {
    string card_number;
    string expiry;
    string verification;
    string pin;
    string network;
};

// ----------------- Helpers -----------------
string clean_string(const string& s) {
    string res;
    for (char c : s)
        if (c != '*' && c != ',')
            res += c;
    return res;
}

string format_card(const string& digits) {
    string d;
    for (char c : digits)
        if (isdigit(c)) d += c;

    if (d.size() != 16) return d;
    return d.substr(0,4) + "-" + d.substr(4,4) + "-" +
           d.substr(8,4) + "-" + d.substr(12,4);
}

// Convert expiry MM/YYYY to numbers
void parse_expiry(const string& expiry, int& month, int& year) {
    month = 0; year = 0;
    char slash;
    stringstream ss(expiry);
    ss >> month >> slash >> year;
}

// ----------------- CSV -----------------

vector<CardRow> read_csv(const string& path) {
    vector<CardRow> rows;
    ifstream file(path);
    if (!file.is_open()) { cerr << "Error opening " << path << endl; return rows; }

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        stringstream ss(line);
        CardRow r;
        getline(ss, r.card_number, ',');
        getline(ss, r.expiry, ',');
        getline(ss, r.verification, ',');
        getline(ss, r.pin, ',');
        getline(ss, r.network, ',');
        rows.push_back(r);
    }
    return rows;
}

void write_csv(const string& path, const vector<CardRow>& rows) {
    ofstream out(path);
    if (!out.is_open()) { cerr << "Error writing " << path << endl; return; }
    out << "Credit Card Number,Expiry Date,Verification Code,PIN,Issueing Network\n";
    for (const auto& r : rows)
        out << r.card_number << "," << r.expiry << "," << r.verification << ","
            << r.pin << "," << r.network << "\n";
}

// ----------------- Radix Sort (linear) -----------------

void counting_sort(vector<CardRow>& arr, int (*key_func)(const CardRow&), int max_key) {
    vector<vector<CardRow>> buckets(max_key + 1);
    for (auto& r : arr) {
        int k = key_func(r);
        buckets[k].push_back(r);
    }
    size_t idx = 0;
    for (auto& bucket : buckets) {
        for (auto& r : bucket) arr[idx++] = r;
    }
}

int get_pin(const CardRow& r) {
    int p = 0;
    try { p = stoi(r.pin); } catch(...) { p = 0; }
    return p;
}

int get_month(const CardRow& r) {
    int m, y;
    parse_expiry(r.expiry, m, y);
    return m;
}

int get_year(const CardRow& r) {
    int m, y;
    parse_expiry(r.expiry, m, y);
    return y;
}

void radix_sort_dump2(vector<CardRow>& arr) {
    counting_sort(arr, get_pin, 9999);      // PIN
    counting_sort(arr, get_month, 12);      // Month
    counting_sort(arr, get_year, 3000);     // Year, 0–3000
}

// ----------------- Linear merge -----------------
vector<CardRow> merge_linear_index(const vector<CardRow>& dump1, const vector<CardRow>& dump2) {
    vector<CardRow> merged;
    merged.reserve(dump1.size());
    for (size_t i = 0; i < dump1.size(); ++i) {
        string full_digits = clean_string(dump1[i].card_number) + clean_string(dump2[i].card_number);
        CardRow r = dump2[i]; // take other fields from dump2
        r.card_number = format_card(full_digits);
        merged.push_back(r);
    }
    return merged;
}

// ----------------- Timing -----------------
template<typename Func>
double timeit(Func f){
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration_cast<duration<double>>(end-start).count();
}

// ----------------- Main -----------------
int main() {
    const string data_path = "assignment_1/data/";
    const string res_path  = "assignment_1/results/";

    auto dump2 = read_csv(data_path + "carddump2.csv");
    if(dump2.empty()) return 1;

    // Sort dump2 using radix sort (linear algorithm)
    double t_sort = timeit([&](){ radix_sort_dump2(dump2); });
    cout << "Radix sort dump2 time: " << t_sort << " s\n";

    write_csv(res_path + "carddump2_sorted.csv", dump2);

    auto dump1 = read_csv(data_path + "carddump1.csv");
    if(dump1.size() != dump2.size()){ cerr << "Row count mismatch\n"; return 1; }

    // Empirical study
    vector<size_t> sizes = {1000,2000,5000,10000,20000};
    cout << "\nEmpirical timing study (linear merge):\n";
    cout << "N\tLinear(s)\n";
    for(auto N : sizes){
        vector<CardRow> d1(dump1.begin(), dump1.begin()+N);
        vector<CardRow> d2(dump2.begin(), dump2.begin()+N);

        double t_linear = timeit([&](){ auto m = merge_linear_index(d1,d2); });
        cout << N << "\t" << t_linear << "\n";
    }

    // Final merge
    auto merged = merge_linear_index(dump1,dump2);
    write_csv(res_path + "carddump_sorted_full.csv", merged);

    cout << "\nFinal merged cards:\n";
    cout << "Credit Card Number,Expiry Date,Verification Code,PIN,Issueing Network\n";
    for(const auto& r:merged)
        cout << r.card_number << "," << r.expiry << "," << r.verification << ","
             << r.pin << "," << r.network << "\n";

    cout << "\nDONE. Files created in assignment_1/results\n";
    return 0;
}
