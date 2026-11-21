#include <cstdio>
#include "Sacco.h"

int main() {
    int ok = 0, n = 0;
    auto check = [&](const string& what, bool pass) { n++; ok += pass; cout << (pass ? "PASS  " : "FAIL  ") << what << "\n"; };
    auto mk = [](int a, double b = 1000) { return Member{a, "M" + to_string(a), "0772000111", b}; };

    // ---- hash table ----
    HashTable h;
    check("H1 find and remove on an empty table", !h.find(1) && !h.remove(1));
    check("H2 insert one record and find it", h.insert(mk(1001)) && h.size() == 1 && h.find(1001)->name == "M1001");
    check("H3 duplicate key and keys 0, -5 rejected", !h.insert(mk(1001)) && !h.insert(mk(0)) && !h.insert(mk(-5)) && h.size() == 1);
    check("H4 absent key not found", !h.find(9999));
    HashTable c;
    c.insert(mk(1005)); c.insert(mk(1106)); c.insert(mk(2217));     // all three go to bucket 96
    check("H5 three colliding keys are all found", hashKey(1005) == hashKey(2217) && c.find(1005) && c.find(1106) && c.find(2217));
    check("H6 remove middle, first and last of a chain", c.remove(1106) && c.find(2217) && c.find(1005) && c.remove(2217) && c.remove(1005) && c.size() == 0);

    // ---- accounts ----
    Sacco s;
    check("S1 add valid; duplicate, bad phone, empty name, negative balance rejected",
          s.addMember(mk(1001, 500)) == "" && s.addMember(mk(1001)) != "" && s.addMember({1002, "A", "077", 0}) != "" &&
          s.addMember({1003, "", "0772000111", 0}) != "" && s.addMember({1004, "A", "0772000111", -1}) != "");
    check("S2 update works; absent account rejected", s.update(1001, "New", "0701000000") == "" && s.search(1001)->name == "New" && s.update(9, "A", "0701000000") != "");
    check("S3 deposit adds; 0, negative and absent rejected", s.deposit(1001, 250) == "" && s.search(1001)->balance == 750 && s.deposit(1001, 0) != "" && s.deposit(1001, -5) != "" && s.deposit(9, 5) != "");
    check("S4 withdraw too much rejected; exact balance leaves 0", s.withdraw(1001, 751) != "" && s.withdraw(1001, 750) == "" && s.search(1001)->balance == 0);
    s.addMember(mk(1002, 1000)); s.addMember(mk(1003, 500));
    check("S5 transfer moves the money", s.transfer(1002, 1003, 300) == "" && s.search(1002)->balance == 700 && s.search(1003)->balance == 800);
    check("S6 same account, absent account, too much: rejected and unchanged",
          s.transfer(1002, 1002, 1) != "" && s.transfer(1002, 9, 1) != "" && s.transfer(1002, 1003, 701) != "" && s.search(1002)->balance == 700);
    check("S7 10% interest works; rate 0 and 101 rejected", s.applyInterest(10) == "" && s.search(1002)->balance > 769.9 && s.applyInterest(0) != "" && s.applyInterest(101) != "");

    // ---- ordered reports ----
    Sacco r;
    int acc[] = {1005, 1001, 1009, 1003, 1007};
    double bal[] = {300, 100, 500, 100, 200};
    for (int i = 0; i < 5; i++) r.addMember(mk(acc[i], bal[i]));   // added out of order
    vector<Member> all = r.listAll(), top = r.topSavers(3), g = r.range(1003, 1007);
    check("R1 list is in account order", all.size() == 5 && all[0].accountNo == 1001 && all[4].accountNo == 1009);
    check("R2 range 1003-1007 gives 3; lo > hi and empty range give none", g.size() == 3 && g[2].accountNo == 1007 && r.range(1009, 1001).empty() && r.range(2000, 3000).empty());
    check("R3 top 3 by balance; n above size gives all; n = 0 gives none", top[0].accountNo == 1009 && top[1].accountNo == 1005 && top[2].accountNo == 1007 && r.topSavers(99).size() == 5 && r.topSavers(0).empty());
    check("R4 delete leaves hash table and tree; absent delete rejected", r.removeMember(1005) == "" && !r.search(1005) && r.listAll().size() == 4 && r.removeMember(1005) != "");
    Sacco e;
    check("R5 reports on an empty register are empty", e.listAll().empty() && e.topSavers(5).empty());

    // ---- files ----
    { ofstream f("t.txt"); f << "1001,Good,0772000111,500\nabc,Bad Id,0772000111,1\n1002,Bad Phone,123,1\n1003,No Balance,0772000111\n\n1001,Dup,0772000111,5\n"; }
    Sacco t, u; int sk;
    check("F1 load skips bad, duplicate and incomplete lines", t.load("t.txt", sk) == 1 && sk == 4);
    t.save("t.txt");
    check("F2 save then load gives the same data; missing file gives -1", u.load("t.txt", sk) == 1 && u.search(1001)->balance == 500 && u.load("none.txt", sk) == -1);
    remove("t.txt");

    cout << "\n" << ok << " of " << n << " tests passed\n";
}