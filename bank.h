#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include "bank_customer.h"

using namespace std;

class Bank {
    private:
        vector<BankCustomer*> customers;
        vector<BankTransaction*> transactions;

    public:

        Bank() = default;

        void registerCustomer(BankCustomer* c) {
            if (!c) {
                return;
            }
            customers.push_back(c);
        }

        void listAllCustomers() const {
            cout << "\n===== Bank Customers =====" << endl;
            for (auto c : customers) {
                cout << "Username: " << c->getUsername()
                    << " | Name: " << c->getName()
                    << " | Balance: " << c->getBalance() << endl;
            }
        }

        void listTransactionsWithinWeek() const {
            cout << "\n===== Bank Transactions (within last 7 days) =====" << endl;
            for(auto c : customers) {
                cout << "\n--- " << c->getUsername() << " ---\n";
                c->listCashFlowMonth();
            }
        }

        void listDormantAccounts() const {
            cout << "\n=== Dormant Accounts (no tx in las month) ===\n";
            cout << "(placeholder - requires checking last tx date per customer)\n";
        }

        void listTopNUsersToday(int n) const {
            struct U { string username; int cnt; };
            vector<U> arr;
            std::time_t now = std::time(nullptr);
            char todayBuf[16];
            std::strftime(todayBuf, sizeof(todayBuf), "%b %d", std::localtime(&now));
            for (auto c : customers) {
                int cnt = 0;
                for (const auto &tx : c->getCashFlow()) {
                    if (tx.date.find(todayBuf) != string::npos) {
                        ++cnt;
                    }
                    arr.push_back({c->getUsername(), cnt});
                }
            }
            std::sort(arr.begin(), arr.end(), [](const U &a, const U &b){ return a.cnt > b.cnt; });
            cout << "\n===== Top " << n << " bank users (today =====\n)";
            for (int i = 0; i<n && i<(int)arr.size(); ++i) {
                cout << arr[i].username << " -> " << arr[i].cnt << " tx\n";
            }
        }
        
        void saveData(std::ofstream &out) {
            out << "[BANK]\n";
        
            // --- Simpan semua customer ---
            out << "[CUSTOMERS]\n";
            for (auto &c : customers) {
                out << c->serialize() << "\n";
            }
        
            // --- Simpan semua transaksi global ---
            out << "[TRANSACTIONS]\n";
            for (auto &t : transactions) {
                out << t->serialize() << "\n";
            }
        }
    
        void loadData(ifstream &in) {
            string line;
            while (getline(in, line)) {
                if (line == "[BUYERS]" || line == "[SELLERS]") {
                    break;
                }
                if (line.empty() || line == "[BANK]") {
                    continue;
                }
                BankCustomer c;
                c.deserialize(line);
                customers.push_back(new BankCustomer(c));
            }
            if (!in.eof()) {
                in.seekg(-line.size()-1, ios::cur);
            }
        }
};

#endif