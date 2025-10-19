#ifndef BUYER_H
#define BUYER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "transaction.h"
#include "bank_customer.h"

class Buyer : public BankCustomer{
    private:
        int id;
        vector<int> orderIds;

    public:
    Buyer() = default;
    Buyer(int id_, const string &username, const string &password, const string &name_,
    const string &email_, const string &phone_, const string &address_, double initialBalance)
        : BankCustomer(username, password, name_, email_, phone_, address_, initialBalance), id(id_) {}

    int getId() const {
        return id;
    }

    void addOrderId(int oid) {
        orderIds.push_back(oid);
    }

    const vector<int>& getOrderIds() const {
        return orderIds;
    }

    void listMyOrders(const vector<Order>& allOrders) const {
        cout << "\n===== Orders for buyer: " << getUsername() << " =====\n";
        for (int oid : orderIds) {
            if (oid >= 0 && oid < (int)allOrders.size()) {
                allOrders[oid].show();
            }
        }
    }

    double spendingLastKDays(const vector<Order>& allOrders, int k) const {
        double sum = 0.0;
        for (int oid : orderIds) {
            if (oid >= 0 && oid < (int)allOrders.size()) {
                const Order &o = allOrders[oid];
                if (o.status != OrderStatus::CANCELLED) {
                    sum += o.total;
                }
            }
        }
        return sum;
    }
    
    string serialize() const {
        return BankCustomer::serialize();
    }

    void deserialize(const string &line) {
        BankCustomer::deserialize(line);
    }
};

#endif