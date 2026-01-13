#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

/* ---------- CSV utils ---------- */

// split by comma
vector<string> split_csv(const string& line) {
    vector<string> out;
    string cell;
    stringstream ss(line);
    while (getline(ss, cell, ',')) out.push_back(cell);
    return out;
}

// split card number by '-'
vector<string> split_dash(const string& s) {
    vector<string> out;
    string part;
    stringstream ss(s);
    while (getline(ss, part, '-')) out.push_back(part);
    return out;
}

/* ---------- key creation ---------- */
// key = YYYY MM PIN → sortable integer
long long make_key(const string& expiry, const string& pin) {
    // expiry format: MM/YYYY
    auto pos = expiry.find('/');
    if (pos == string::npos) return 0;

    int month = stoi(expiry.substr(0, pos));
    int year  = stoi(expiry.substr(pos + 1));
    int p     = stoi(pin);

    return (long long)year * 1000000LL + month * 10000LL + p;
}

/* ---------- card merge ---------- */
// dump1: nnnn-nnnn-nnnn-****
// dump2: ****-****-****-nnnn
string merge_cards(const string& a, const string& b) {
    auto pa = split_dash(a);
    auto pb = split_dash(b);

    if (pa.size() != 4 || pb.size() != 4)
        throw runtime_error("Invalid card format");

    return pa[0] + "-" + pa[1] + "-" + pa[2] + "-" + pb[3];
}

/* ---------- main ---------- */
int main() {
    const string dataPath   = "assignment_1/data/";
    const string resultPath = "assignment_1/results1/";

    /* ===== READ carddump2 ===== */
    ifstream f2(dataPath + "carddump2.csv");
    if (!f2.is_open()) {
        cerr << "Cannot open carddump2.csv\n";
        return 1;
    }

    string header;
    getline(f2, header);
    auto headers = split_csv(header);

    vector<vector<string>> rows2;
    string line;
    while (getline(f2, line)) {
        rows2.push_back(split_csv(line));
    }
    f2.close();

    // find columns
    int colCard = -1, colExpiry = -1, colPin = -1;
    for (int i = 0; i < headers.size(); i++) {
        if (headers[i] == "Credit Card Number") colCard = i;
        if (headers[i] == "Expiry Date") colExpiry = i;
        if (headers[i] == "PIN") colPin = i;
    }

    if (colCard < 0 || colExpiry < 0 || colPin < 0) {
        cerr << "Required columns not found\n";
        return 1;
    }

    /* ===== SORT carddump2 ===== */
    sort(rows2.begin(), rows2.end(), [&](auto& a, auto& b) {
        return make_key(a[colExpiry], a[colPin]) <
               make_key(b[colExpiry], b[colPin]);
    });

    /* ===== WRITE sorted carddump2 ===== */
    ofstream out2(resultPath + "carddump2_sorted.csv");
    out2 << header << "\n";
    for (auto& r : rows2) {
        for (int i = 0; i < r.size(); i++) {
            if (i) out2 << ",";
            out2 << r[i];
        }
        out2 << "\n";
    }
    out2.close();

    /* ===== READ carddump1 ===== */
    ifstream f1(dataPath + "carddump1.csv");
    if (!f1.is_open()) {
        cerr << "Cannot open carddump1.csv\n";
        return 1;
    }

    getline(f1, header);
    vector<vector<string>> rows1;
    while (getline(f1, line)) {
        rows1.push_back(split_csv(line));
    }
    f1.close();

    if (rows1.size() != rows2.size()) {
        cerr << "Row count mismatch\n";
        return 1;
    }

    /* ===== MERGE ===== */
    vector<vector<string>> merged = rows2;
    for (size_t i = 0; i < merged.size(); i++) {
        merged[i][colCard] =
            merge_cards(rows1[i][colCard], rows2[i][colCard]);
    }

    /* ===== WRITE FINAL CSV ===== */
    ofstream outFinal(resultPath + "carddump_sorted_full.csv");
    outFinal << header << "\n";
    for (auto& r : merged) {
        for (int i = 0; i < r.size(); i++) {
            if (i) outFinal << ",";
            outFinal << r[i];
        }
        outFinal << "\n";
    }
    outFinal.close();

    /* ===== PRINT TO CONSOLE ===== */
    cout << header << "\n";
    for (auto& r : merged) {
        for (int i = 0; i < r.size(); i++) {
            if (i) cout << ", ";
            cout << r[i];
        }
        cout << "\n";
    }

    cout << "\nDone. Files created in assignment_1/results1/\n";
    return 0;
}
