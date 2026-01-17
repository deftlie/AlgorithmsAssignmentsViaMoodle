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

void parse_expiry(const string& expiry, int& month, int& year) {
    month = 0; year = 0;
    char slash;
    stringstream ss(expiry);
    ss >> month >> slash >> year;
}

int safe_stoi(const string& s){
    try { return stoi(s); }
    catch(...) { return 0; }
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
    for (auto& bucket : buckets)
        for (auto& r : bucket)
            arr[idx++] = r;
}

int get_pin(const CardRow& r) { return safe_stoi(r.pin); }
int get_month(const CardRow& r) { int m,y; parse_expiry(r.expiry,m,y); return m; }
int get_year(const CardRow& r)  { int m,y; parse_expiry(r.expiry,m,y); return y; }

void radix_sort_dump2(vector<CardRow>& arr) {
    counting_sort(arr, get_pin, 9999);
    counting_sort(arr, get_month, 12);
    counting_sort(arr, get_year, 3000);
}

// ----------------- Linear merge -----------------
vector<CardRow> merge_linear_index(const vector<CardRow>& dump1, const vector<CardRow>& dump2) {
    vector<CardRow> merged;
    merged.reserve(dump1.size());
    for (size_t i = 0; i < dump1.size(); ++i) {
        string full_digits = clean_string(dump1[i].card_number) + clean_string(dump2[i].card_number);
        CardRow r = dump2[i];
        r.card_number = format_card(full_digits);
        merged.push_back(r);
    }
    return merged;
}

// ----------------- Log-linear merge -----------------
vector<CardRow> merge_loglinear(vector<CardRow> d1, vector<CardRow> d2) {
    auto cmp = [](const CardRow& a, const CardRow& b) {
        int y1,m1,y2,m2;
        parse_expiry(a.expiry,m1,y1); parse_expiry(b.expiry,m2,y2);
        if(y1!=y2) return y1<y2;
        if(m1!=m2) return m1<m2;
        return safe_stoi(a.pin) < safe_stoi(b.pin);
    };

    stable_sort(d1.begin(), d1.end(), cmp);
    stable_sort(d2.begin(), d2.end(), cmp);

    vector<CardRow> merged;
    merged.reserve(d1.size());
    for(size_t i=0;i<d1.size();++i){
        string full_digits = clean_string(d1[i].card_number) + clean_string(d2[i].card_number);
        CardRow r = d2[i];
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

    auto dump2_orig = read_csv(data_path + "carddump2.csv");
    if(dump2_orig.empty()) return 1;
    auto dump1_orig = read_csv(data_path + "carddump1.csv");
    if(dump1_orig.size() != dump2_orig.size()){ cerr << "Row count mismatch\n"; return 1; }

    // ----------------- Empirical timing -----------------
    vector<size_t> sizes = {1000,2000,5000,10000,20000};

    cout << "\nEmpirical timing study (Linear vs Log-linear):\n";
    cout << "N\tLinear(s)\tLogLinear(s)\n";

    for(auto N : sizes){
        vector<CardRow> d1(dump1_orig.begin(), dump1_orig.begin()+N);
        vector<CardRow> d2(dump2_orig.begin(), dump2_orig.begin()+N);

        // Linear merge (radix sort + merge)
        double t_linear = timeit([&](){
            vector<CardRow> temp_d2 = d2;
            radix_sort_dump2(temp_d2);
            auto m = merge_linear_index(d1,temp_d2);
        });

        // Log-linear merge
        double t_loglinear = timeit([&](){
            auto m = merge_loglinear(d1,d2);
        });

        cout << N << "\t" << t_linear << "\t" << t_loglinear << "\n";
    }

    // ----------------- Final linear merge dump1 + dump2 ----------
    vector<CardRow> sorted_d2 = dump2_orig;
    radix_sort_dump2(sorted_d2);
    auto final_merged = merge_linear_index(dump1_orig, sorted_d2);


    cout << "\n=== Final merged dump1 + dump2 ===\n";
    cout << "Credit Card Number,Expiry Date,Verification Code,PIN,Issueing Network\n";
    for (const auto& r : final_merged) {
        cout << r.card_number << "," << r.expiry << "," << r.verification << ","
             << r.pin << "," << r.network << "\n";
    }

    write_csv(res_path + "carddump_sorted_full.csv", final_merged);
    cout << "\nFinal merged CSV created: " << res_path + "carddump_sorted_full.csv\n";
    cout << "DONE.\n";

    return 0;
}
