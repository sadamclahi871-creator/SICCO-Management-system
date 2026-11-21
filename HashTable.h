#pragma once
#include <iostream>
#include <string>
using namespace std;

struct Member { int accountNo; string name, phone; double balance; };   // accountNo is the key
struct Node { Member data; Node* next; };   // one link in a bucket's chain

const int TABLE_SIZE = 101;     // number of buckets (prime)
inline int hashKey(int acc) { return acc % TABLE_SIZE; }   // account number -> bucket 0..100

// Our own hash table, using separate chaining for collisions
class HashTable {
    Node* bucket[TABLE_SIZE] = {};  // every bucket starts empty (nullptr)
    int count = 0;
public:
    HashTable() = default;
    HashTable(const HashTable&) = delete;    // copying disabled: two tables would free the same nodes
    HashTable& operator=(const HashTable&) = delete;
    ~HashTable() {    // free every node
        for (Node*& head : bucket)
            while (head) { Node* next = head->next; delete head; head = next; }
    }

    bool insert(const Member& m) {
        if (m.accountNo <= 0 || find(m.accountNo)) return false;   // bad key or duplicate
        int i = hashKey(m.accountNo);
        bucket[i] = new Node{m, bucket[i]};         // new node goes to the front of the chain
        count++;
        return true;
    }

    Member* find(int acc) {                         // pointer to the stored record, or nullptr
        if (acc <= 0) return nullptr;
        for (Node* p = bucket[hashKey(acc)]; p; p = p->next)
            if (p->data.accountNo == acc) return &p->data;
        return nullptr;
    }

    bool remove(int acc) {
        if (acc <= 0) return false;
        int i = hashKey(acc);
        Node* prev = nullptr;
        for (Node* p = bucket[i]; p; prev = p, p = p->next) {
            if (p->data.accountNo != acc) continue;
            (prev ? prev->next : bucket[i]) = p->next;   // unlink: skip over p
            delete p;
            count--;
            return true;
        }
        return false;
    }

    int size() const { return count; }

    void show() const {                             // print the chains (good for the demo)
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (!bucket[i]) continue;
            cout << "Bucket " << i << ":";
            for (Node* p = bucket[i]; p; p = p->next) cout << " " << p->data.accountNo;
            cout << "\n";
        }
    }
};