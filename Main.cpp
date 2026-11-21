#include "Sacco.h"

struct InputEnded {}; // thrown if the input closes, so we still exit safely

string ask(const string& prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) throw InputEnded();
    return s;
}

int askInt(const string& p) {                      // keep asking until a whole number is typed
    while (true) {
        try { string s = ask(p); size_t used; int v = stoi(s, &used); if (used == s.size()) return v; }
        catch (const logic_error&) {}              // stoi failed: not a number
        cout << "Invalid number.\n";
    }
}

double askNum(const string& p) {
    while (true) {
        try { string s = ask(p); size_t used; double v = stod(s, &used); if (used == s.size()) return v; }
        catch (const logic_error&) {}
        cout << "Invalid amount.\n";
    }
}

string askText(const string& p, bool phone) {     // name: not empty. phone: exactly 10 digits
    while (true) {
        string s = ask(p);
        if (phone ? validPhone(s) : !s.empty()) return s;
        cout << (phone ? "Phone must be 10 digits.\n" : "Cannot be empty.\n");
    }
}

void print(const vector<Member>& v) {
    if (v.empty()) { cout << "Nothing to show.\n"; return; }
    cout << left << setw(10) << "Account" << setw(22) << "Name" << setw(14) << "Phone" << "Balance\n";
    for (const Member& m : v)
        cout << setw(10) << m.accountNo << setw(22) << m.name << setw(14) << m.phone << m.balance << "\n";
}

void say(const string& err, const string& ok) { cout << (err.empty() ? ok : "Error: " + err) << "\n"; }

int main() {
    Sacco s;
    cout << fixed << setprecision(2);
    int skipped, loaded = s.load("members.txt", skipped);
    if (loaded < 0) cout << "members.txt not found, starting empty.\n";
    else cout << "Loaded " << loaded << " members, skipped " << skipped << ".\n";

    int choice = -1;
    try {
        while (choice != 0) {
            cout << "\n===== SACCO MEMBER ACCOUNT MANAGER =====\n"
                    " 1. Add member\n"
                    " 2. Search member\n"
                    " 3. Update member\n"
                    " 4. Delete/Remove member\n"
                    " 5. Deposit\n"
                    " 6. Withdraw\n"
                    " 7. Transfer\n"
                    " 8. Apply interest\n"
                    " 9. Report: account number range\n"
                    "10. Report: top savers\n"
                    "11. Save\n"
                    "12. List all members\n"
                    "13. Show hash buckets\n"
                    " 0. Save and exit\n";
            choice = askInt("Choose: ");
            switch (choice) {//switches
            case 1: { int a = askInt("Account no: "); string n = askText("Name: ", false), p = askText("Phone (10 digits): ", true);
                      say(s.addMember({a, n, p, askNum("Opening balance: ")}), "Member added."); break; }
            case 2: { Member* m = s.search(askInt("Account no: "));
                      if (m) print({*m}); else cout << "Account not found.\n";
                      break; }
            case 3: { int a = askInt("Account no: "); string n = askText("New name: ", false);
                      say(s.update(a, n, askText("New phone: ", true)), "Member updated."); break; }
            case 4: say(s.removeMember(askInt("Account no: ")), "Member deleted."); break;
            case 5: case 6: { int a = askInt("Account no: "); double x = askNum("Amount: ");
                      string err = choice == 5 ? s.deposit(a, x) : s.withdraw(a, x);
                      if (err.empty()) cout << "Done. New balance: " << s.search(a)->balance << "\n";
                      else cout << "Error: " << err << "\n";
                      break; }
            case 7: { int f = askInt("From account: "), t = askInt("To account: ");
                      say(s.transfer(f, t, askNum("Amount: ")), "Transfer done."); break; }
            case 8: say(s.applyInterest(askNum("Interest rate (%): ")), "Interest applied."); break;
            case 9: { int lo = askInt("From account: "); print(s.range(lo, askInt("To account: "))); break; }
            case 10: { int n = askInt("How many top savers? ");
                       if (n > 0) print(s.topSavers(n)); else cout << "Enter a number above 0.\n";
                       break; }
            case 11: cout << (s.save("members.txt") ? "Saved.\n" : "Could not save.\n"); break;
            case 12: print(s.listAll()); break;
            case 13: s.showBuckets(); break;
            case 0: break;
            default: cout << "Choose a number from the menu.\n";
            }
        }
    } catch (InputEnded&) { cout << "\nInput ended.\n"; }

    cout << (s.save("members.txt") ? "Saved. Goodbye!\n" : "Could not save.\n");
    return 0;                                      // destructors free every node here
}