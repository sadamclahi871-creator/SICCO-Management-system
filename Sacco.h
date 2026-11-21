#pragma once
#include <fstream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <set>
#include <vector>
#include <algorithm>
#include "HashTable.h"

inline bool validPhone(const string& p) {          // exactly 10 digits
    return p.size() == 10 && all_of(p.begin(), p.end(), [](char c) { return c >= '0' && c <= '9'; });
}

// The register: hash table for the data + balanced tree (std::set) for the order.
// Operations return "" on success, or a message saying what went wrong.
class Sacco {
    HashTable table;  // our hash table: holds the records
    set<int> index;  // balanced tree: account numbers in sorted order.
public:
    string addMember(const Member& m) {
        if (m.name.empty() || !validPhone(m.phone) || m.balance < 0) return "Invalid name, phone (10 digits) or balance";
        if (!table.insert(m)) return "Account number is invalid or already exists";
        index.insert(m.accountNo);                 // keep the tree in step with the hash table
        return "";
    }

    Member* search(int acc) { return table.find(acc); }

    string update(int acc, const string& name, const string& phone) {
        Member* m = table.find(acc);
        if (!m) return "Account not found";
        if (name.empty() || !validPhone(phone)) return "Invalid name or phone";
        m->name = name;
        m->phone = phone;
        return "";
    }

    string removeMember(int acc) {
        if (!table.remove(acc)) return "Account not found";
        index.erase(acc);                          // remove from the tree too
        return "";
    }

    string deposit(int acc, double amt) {
        Member* m = table.find(acc);
        if (amt <= 0) return "Amount must be positive";
        if (!m) return "Account not found";
        m->balance += amt;
        return "";
    }

    string withdraw(int acc, double amt) {
        Member* m = table.find(acc);
        if (amt <= 0) return "Amount must be positive";
        if (!m) return "Account not found";
        if (m->balance < amt) return "Insufficient funds";
        m->balance -= amt;
        return "";
    }

    string transfer(int from, int to, double amt) {
        Member *a = table.find(from), *b = table.find(to);
        if (amt <= 0) return "Amount must be positive";
        if (from == to) return "Cannot transfer to the same account";
        if (!a || !b) return "Account not found";
        if (a->balance < amt) return "Insufficient funds";
        a->balance -= amt;                         // all checks passed, now move the money
        b->balance += amt;
        return "";
    }

    string applyInterest(double rate) {
        if (rate <= 0 || rate > 100) return "Rate must be above 0 and at most 100";
        for (int acc : index) table.find(acc)->balance *= 1 + rate / 100;
        return "";
    }

    // Ordered reports: the tree gives the order, the hash table gives the data
    vector<Member> range(int lo, int hi) {
        vector<Member> out;
        for (auto it = index.lower_bound(lo); it != index.end() && *it <= hi; ++it)
            out.push_back(*table.find(*it));
        return out;
    }

    vector<Member> listAll() { return range(1, INT_MAX); }

    vector<Member> topSavers(int n) {
        vector<Member> v = listAll();
        stable_sort(v.begin(), v.end(), [](const Member& x, const Member& y) { return x.balance > y.balance; });
        if (n < (int)v.size()) v.resize(max(n, 0));     // highest balance first, ties keep account order
        return v;
    }

    int size() const { return table.size(); }
    void showBuckets() const { table.show(); }

    // File format, one member per line: accountNo,name,phone,balance
    int load(const string& file, int& skipped) {   // returns members loaded, or -1 if no file
        ifstream in(file);
        skipped = 0;
        if (!in) return -1;
        string line;
        int loaded = 0;
        while (getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();   // Windows line ending
            if (line.empty()) continue;
            stringstream ss(line);
            string acc, name, phone, bal;      // fresh strings for every line
            getline(ss, acc, ','); getline(ss, name, ','); getline(ss, phone, ','); getline(ss, bal);
            try {
                if (addMember({stoi(acc), name, phone, stod(bal)}) == "") loaded++;
                else skipped++;                    // invalid or duplicate record
            } catch (...) { skipped++; }           // a number was expected but not found
        }
        return loaded;
    }

    bool save(const string& file) {
        ofstream out(file);
        out << fixed << setprecision(2);
        for (int acc : index) {
            Member* m = table.find(acc);
            out << acc << "," << m->name << "," << m->phone << "," << m->balance << "\n";
        }
        return bool(out);
    }
};