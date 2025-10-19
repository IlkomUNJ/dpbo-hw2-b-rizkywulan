#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace std;

struct BankTransaction {
    string type;
    double amount;
    string date;
    string description;

    std::string serialize() const {
        std::ostringstream oss;
        oss << type << "|" << amount << "|" << date << "|" << description;
        return oss.str();
    }

    void deserialize(const std::string &line) {
        std::istringstream iss(line);
        std::getline(iss, type, '|');
        std::string amt;
        std::getline(iss, amt, '|');
        amount = std::stod(amt);
        std::getline(iss, date, '|');
        std::getline(iss, description, '|');
    }
};

class BankCustomer {
    private:
        string name;
        double balance;
        string username;
        string password;
        string email;
        string phone;
        string address;
        vector<BankTransaction> cashFlow;

    public:

        BankCustomer() : balance(0.0) {}
        BankCustomer(const string &u, const string &p, const string &n,
                    const string &e, const string &pn, const string &a, double b)
        : username(u), password(p), name(n), email(e), phone(pn), address(a), balance(b) {
            if (b > 0.0) {
                recordTransaction("credit", b, "Initial deposit");
            }
        }

        const string& getUsername() const {
            return username;
        }
        void setUsername(const string &u) {
            username = u;
        }
        const string& getPassword() const {
            return password;
        }
        void setPassword(const string &p) {
            password = p;
        }
        const string& getName() const {
            return name;
        }
        const string& getEmail() const {
            return email;
        }
        const string& getPhone() const {
            return phone;
        }
        const string& getAddress() const {
            return address;
        }
        double getBalance() const {
            return balance;
        }
        void setBalance(double b) {
            balance = b;
        }

        const vector<BankTransaction>& getCashFlow() const {
            return cashFlow;
        }

        void topUp(double amount) {
            if (amount <= 0) {
                return;
            }
            balance += amount;
            recordTransaction("credit", amount, "Top-up");
        }

        bool withdraw(double amount) {
            if (amount <= 0) {
                return false;
            }
            if (amount > balance) {
                return false;
            }
            balance -= amount;
            recordTransaction("debit", amount, "Withdraw");
            return true;
        }

        void recordTransaction(const string &type, double amount, const string &description) {
            std::time_t now = std::time(nullptr);
            char buf[64];
            std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S %Y", std::localtime(&now));
            BankTransaction t{type, amount, string(buf), description};
            cashFlow.push_back(t);
        }

        void listCashFlowToday() const {
            std:: time_t now = std::time(nullptr);
            char todayBuf[16];
            std::strftime(todayBuf, sizeof(todayBuf), "%b %d", std::localtime(&now));
            cout << "\n===== Cash Flow (Today) for " << username << " =====\n";
            for (const auto &tx : cashFlow) {
                if (tx.date.find(todayBuf) != string::npos) {
                    cout << tx.date << " | " << tx.type << " | " <<
                    std::fixed << std::setprecision(2) << tx.amount << " | " << tx.description << endl;
                }
            }
        }

        void listCashFlowMonth() const {
            std:: time_t now = std::time(nullptr);
            char monthBuf[8];
            std::strftime(monthBuf, sizeof(monthBuf), "%b", std::localtime(&now));
            cout << "\n===== Cash Flow (This Month) for " << username << " =====\n";
            for (const auto &tx : cashFlow) {
                if (tx.date.find(monthBuf) != string::npos) {
                    cout << tx.date << " | " << tx.type << " | " <<
                    std::fixed << std::setprecision(2) << tx.amount << " | " << tx.description << endl;
                }
            }
        }

        virtual ~BankCustomer() = default;

        string serialize() const {
            return username + "|" + password + "|" + name + "|" + phone + "|" + email + "|" + address + "|" + to_string(balance);
        }
    
        void deserialize(const string& data) {
            if (data.empty() || data[0] == '[') return; // abaikan header seperti [BANKCUSTOMER]
            
            vector<string> parts;
            size_t start = 0, end;
            while ((end = data.find('|', start)) != string::npos) {
                parts.push_back(data.substr(start, end - start));
                start = end + 1;
            }
            parts.push_back(data.substr(start)); // last part (balance)
        
            if (parts.size() < 7) {
                cerr << "Format BankCustomer salah: " << data << endl;
                return;
            }
        
            username = parts[0];
            password = parts[1];
            name     = parts[2];
            phone    = parts[3];
            address  = parts[4];
            email    = parts[5];
            try {
                balance = stod(parts[6]);
            } catch (...) {
                cerr << "Gagal konversi balance: " << parts[6] << endl;
                balance = 0;
            }
        }
};

#endif