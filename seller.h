#ifndef SELLER_H
#define SELLER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "bank_customer.h"
#include "item.h"
#include "transaction.h"

using namespace std;

class Seller : public BankCustomer {
    private:
        int id;
        string storeName;
        vector<int> itemIds;
        vector<int> orderIds;

    public:

        Seller() = default;

        Seller(int id_, const string &username, const string &password, const string &storeName_, 
            const string &email_, const string &phone_, const string &address_, double initialBalance)
            : BankCustomer(username, password, storeName_, email_, phone_, address_, initialBalance),
            id(id_), storeName(storeName_) {}

        int getId() const {
            return id;
        }

        const string& getStoreName() const {
            return storeName;
        }

        void addItemId(int iid) {
            itemIds.push_back(iid);
        }

        const vector<int>& getItemIds() const {
            return itemIds;
        }

        void addOrderId(int oid) {
            orderIds.push_back(oid);
        } 
        const vector<int>& getOrderId() const {
            return orderIds;
        }

        void listMyItems(const vector<Item> &allItems) const {
            cout << "\n===== Items for seller: " << storeName << " (user: " << getUsername() << " ) =====\n";
            for (int iid : itemIds) {
                if (iid >= 0 && iid < (int)allItems.size()) {
                    allItems[iid].show();
                }
            } 
        }

        void listMyOrders(const vector<Order>& allOrders) const {
            cout << "\n===== Orders for seller: " << storeName << " =====\n";
            for (int oid : orderIds) {
                if (oid >= 0 && oid < (int)allOrders.size()) {
                    allOrders[oid].show();
                }
            }
        }

    string serialize() const {
        std::ostringstream oss;
        oss << getUsername() << "|"
            << getPassword() << "|"
            << storeName << "|"
            << getBalance();
        return oss.str();
        //return BankCustomer::serialize();
    }
    
    void deserialize(const std::string &line) {
        std::istringstream iss(line);
        std::string token;
    
        // username
        std::getline(iss, token, '|');
        setUsername(token);
    
        // password
        std::getline(iss, token, '|');
        setPassword(token);
    
        // storeName
        std::getline(iss, storeName, '|');
    
        // balance (cek aman)
        std::getline(iss, token, '|');
        if (!token.empty()) {
            try {
                setBalance(std::stod(token));
            } catch (...) {
                setBalance(0.0);
            }
        } else {
            setBalance(0.0);
        }
    }
};

#endif