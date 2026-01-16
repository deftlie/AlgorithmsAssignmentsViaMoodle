#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

struct CardRow {
    string card_number;
    string expiry;
    string verification;
    string pin;
    string network;
};

// remove '*' and commas (for final merge)
string clean_string(const string& s) {
    string res;
    for (char c : s)
        if (c != '*' && c != ',')
            res += c;
    return res;
}

// format 16-digit card number as XXXX-XXXX-XXXX-XXXX
string format_card(const string& digits) {
    string d;
    for (char c : digits)
        if (isdigit(c)) d += c;

    if (d.size() != 16) return d;

    return d.substr(0,4) + "-" + d.substr(4,4) + "-" +
           d.substr(8,4) + "-" + d.substr(12,4);
}

// create sorting key: YYYYMM + PIN
long long make_key(const string& expiry, const string& pin) {
    if (expiry.empty() || pin.empty()) return 0;
    int mm=0, yyyy=0;
    char slash;
    stringstream ss(expiry);
    if (!(ss >> mm >> slash >> yyyy)) return 0;
    int pin_i = 0;
    try { pin_i = stoi(pin); } catch(...) { pin_i = 0; }
    return (long long)yyyy * 1000000LL + mm * 10000LL + pin_i;
}

// ---------- CSV ----------

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

// ---------- Linear merge by index ----------

vector<CardRow> merge_linear_index(const vector<CardRow>& dump1, const vector<CardRow>& dump2) {
    vector<CardRow> merged;
    for (size_t i = 0; i < dump1.size(); ++i) {
        string full_digits = clean_string(dump1[i].card_number) + clean_string(dump2[i].card_number);
        CardRow r = dump2[i]; // take other fields from dump2
        r.card_number = format_card(full_digits);
        merged.push_back(r);
    }
    return merged;
}

// ---------- Log-linear merge ----------

vector<CardRow> merge_loglinear(const vector<CardRow>& dump1, const vector<CardRow>& dump2) {
    auto d1=dump1; auto d2=dump2;
    auto cmp = [](const CardRow& a, const CardRow& b){ return make_key(a.expiry,a.pin) < make_key(b.expiry,b.pin); };
    sort(d1.begin(), d1.end(), cmp);
    sort(d2.begin(), d2.end(), cmp);

    vector<CardRow> merged;
    for (size_t i=0;i<d1.size();++i) {
        string full_digits = clean_string(d1[i].card_number) + clean_string(d2[i].card_number);
        CardRow r=d2[i];
        r.card_number = format_card(full_digits);
        merged.push_back(r);
    }
    return merged;
}

// ---------- Timing helper ----------

template<typename Func>
double timeit(Func f){
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration_cast<duration<double>>(end-start).count();
}

int main() {
    const string data_path = "assignment_1/data/";
    const string res_path  = "assignment_1/results/";

    auto dump2 = read_csv(data_path + "carddump2.csv");
    if(dump2.empty()) return 1;

    //Sort carddump2 and write sorted CSV
    stable_sort(dump2.begin(), dump2.end(), [](const CardRow& a,const CardRow& b){
        return make_key(a.expiry,a.pin) < make_key(b.expiry,b.pin);
    });
    write_csv(res_path + "carddump2_sorted.csv", dump2);

    auto dump1 = read_csv(data_path + "carddump1.csv");
    if(dump1.size() != dump2.size()){ cerr << "Row count mismatch\n"; return 1; }

    //Empirical study
    vector<size_t> sizes = {1000,2000,5000,10000,20000};
    cout << "\nEmpirical timing study (linear vs log-linear):\n";
    cout << "N\tLinear(s)\tLogLinear(s)\n";
    for(auto N : sizes){
        vector<CardRow> d1(dump1.begin(), dump1.begin()+N);
        vector<CardRow> d2(dump2.begin(), dump2.begin()+N);

        double t_linear = timeit([&](){ auto m = merge_linear_index(d1,d2); });
        double t_loglinear = timeit([&](){ auto m = merge_loglinear(d1,d2); });

        cout << N << "\t" << t_linear << "\t" << t_loglinear << "\n";
    }

    //Final merge
    auto merged = merge_linear_index(dump1,dump2);
    write_csv(res_path + "carddump_sorted_full.csv", merged);

    //Console output
    cout << "\nFinal merged cards:\n";
    cout << "Credit Card Number,Expiry Date,Verification Code,PIN,Issueing Network\n";
    for(const auto& r:merged)
        cout << r.card_number << "," << r.expiry << "," << r.verification << ","
             << r.pin << "," << r.network << "\n";

    cout << "\nDONE. Files created in assignment_1/results\n";
    return 0;
}
