#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Structure to store a credit card record
struct CardRecord {
    string cardNumber;
    string expiryDate; // format MM/YYYY
    string pin;
    string cvv;

    // Other fields can be added if present in CSV
};

// Function to create a numeric key for sorting: YYYYMM + PIN
long long createKey(const CardRecord& rec) {
    int month, year, pin;
    char sep;
    istringstream iss(rec.expiryDate);
    iss >> month >> sep >> year;
    pin = stoi(rec.pin);
    return static_cast<long long>(year) * 10000 + month * 100 + pin;
}

// Function to split card parts and merge
string halfMixer(const string& f, const string& s) {
    vector<string> partsF, partsS;
    string temp;
    istringstream issF(f);
    while (getline(issF, temp, '-')) partsF.push_back(temp);
    istringstream issS(s);
    while (getline(issS, temp, '-')) partsS.push_back(temp);
    return partsF[0]+"-"+partsF[1]+"-"+partsF[2]+"-"+partsS[3];
}

// Read CSV into vector of CardRecord
vector<CardRecord> readCSV(const string& filepath, bool hasHeader=true) {
    vector<CardRecord> records;
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filepath << endl;
        return records;
    }
    string line;
    if (hasHeader) getline(file, line); // skip header
    while (getline(file, line)) {
        istringstream iss(line);
        string token;
        CardRecord rec;
        // assuming CSV: CardNumber,ExpiryDate,PIN,CVV
        getline(iss, rec.cardNumber, ',');
        getline(iss, rec.expiryDate, ',');
        getline(iss, rec.pin, ',');
        getline(iss, rec.cvv, ',');
        records.push_back(rec);
    }
    return records;
}

// Write CSV from vector of CardRecord
void writeCSV(const string& filepath, const vector<CardRecord>& records) {
    ofstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filepath << endl;
        return;
    }
    file << "Credit Card Number,Expiry Date,PIN,CVV\n";
    for (const auto& rec : records) {
        file << rec.cardNumber << "," << rec.expiryDate << "," << rec.pin << "," << rec.cvv << "\n";
    }
    file.close();
}

int main() {
    // Ensure results folder exists
    fs::create_directories("assignment_1/results");

    // 1. Read carddump2, sort, and write sorted CSV
    auto dump2 = readCSV("assignment_1/data/carddump2.csv");
    sort(dump2.begin(), dump2.end(), [](const CardRecord& a, const CardRecord& b){
        return createKey(a) < createKey(b);
    });
    string sortedDump2Path = "assignment_1/results/carddump2_sorted.csv";
    writeCSV(sortedDump2Path, dump2);
    cout << "Sorted carddump2 written to: " << sortedDump2Path << endl;

    // 2. Read carddump1
    auto dump1 = readCSV("assignment_1/data/carddump1.csv");

    // 3. Merge card numbers
    if (dump1.size() != dump2.size()) {
        cerr << "Mismatch in row counts between dump1 and dump2!" << endl;
        return 1;
    }

    vector<CardRecord> finalList;
    for (size_t i = 0; i < dump1.size(); ++i) {
        CardRecord rec = dump2[i]; // take all other fields from sorted dump2
        rec.cardNumber = halfMixer(dump1[i].cardNumber, dump2[i].cardNumber);
        finalList.push_back(rec);
    }

    // 4. Write final merged CSV
    string finalPath = "assignment_1/results/carddump_sorted_full.csv";
    writeCSV(finalPath, finalList);
    cout << "Merged card list written to: " << finalPath << endl;

    // 5. Print fully merged list
    cout << "\nFull merged list:\n";
    for (const auto& rec : finalList) {
        cout << rec.cardNumber << "," << rec.expiryDate << "," << rec.pin << "," << rec.cvv << "\n";
    }

    return 0;
}
