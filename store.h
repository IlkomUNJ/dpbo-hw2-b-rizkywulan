#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include "buyer.h"
#include "seller.h"
#include "transaction.h"
#include "item.h"
#include "bank.h"

using namespace std;

class Store {
    private:
        int nextBuyerId = 0;
        int nextSellerId = 0;
        int nextItemId = 0;
        int nextOrderId = 0;

        vector<Buyer> buyers;
        vector<Seller> sellers;
        vector<Item> items;
        vector<Order> orders;

        Bank bank;

        int findBuyerIndexByUsername(const string &username) const {
            for (int i = 0; i < (int)buyers.size(); i++) {
                if (buyers[i].getUsername()==username) {
                    return i;
                }
            }
            return -1;
        }

        int findSellerIndexByUsername(const string &username) const {
            for (int i = 0; i < (int)sellers.size();i++) {
                if (sellers[i].getUsername()==username) {
                    return i;
                }
            }
            return -1;
        }

    public:
        Store() = default;

        int registerBuyer(const string &username, const string &password, const string &fullName,
        const string &phone, const string &address, const string &email, double initialBalance) {
            if (findBuyerIndexByUsername(username) != -1 || findSellerIndexByUsername(username) != -1) {
                return -1;
            }
            int id = nextBuyerId++;
            buyers.emplace_back(id, username, password, fullName, email, phone, address, initialBalance);
            bank.registerCustomer(&buyers.back());
            return id;
        }

        int registerSeller(const string &username, const string &password, const string &storeName,
            const string &phone, const string &address, const string &email, double initialBalance) {
                if (findBuyerIndexByUsername(username) != -1 || findSellerIndexByUsername(username) != -1) {
                    return -1;
                }
                int id = nextSellerId++;
                sellers.emplace_back(id, username, password, storeName, email, phone, address, initialBalance);
                bank.registerCustomer(&sellers.back());
                return id;
            }

        int loginBuyer(const string &username, const string &password) const {
            for (int i = 0; i < (int)buyers.size();i++) {
                if (buyers[i].getUsername()==username && buyers[i].getPassword()==password) {
                    return i;
                }
            }
            return -1;
        }
        
        int loginSeller(const string &username, const string &password) const {
            for (int i = 0; i < (int)sellers.size();i++) {
                if (sellers[i].getUsername()==username && sellers[i].getPassword()==password) {
                    return i;
                }
            }
            return -1;
        }

        int createItem(int sellerIndex, const string &name, const string &desc, double price, int stock) {
            if (sellerIndex < 0 || sellerIndex >= (int)sellers.size()) {
                return -1;
            }
            int id = nextItemId++;
            items.emplace_back(id, name, desc, price, stock, sellers[sellerIndex].getId());
            items.back().sellerId = sellers[sellerIndex].getId();
            sellers[sellerIndex].addItemId(id);
            return id;
        }

        bool replenishItem(int sellerIndex, int itemIds, int qty) {
            if (sellerIndex < 0 || sellerIndex >= (int)sellers.size()) {
                return false;
            }
            for (auto &it : items) {
                if (it.id == itemIds && it.sellerId==sellers[sellerIndex].getId()) {
                    it.addStock(qty);
                    return true;
                }
            }
            return false;
        }

        bool discardItem(int sellerIndex, int itemId) {
            if (sellerIndex < 0 || sellerIndex >= (int)sellers.size()) {
                return false;
            }
            for (auto &it : items) {
                if (it.id == itemId && it.sellerId==sellers[sellerIndex].getId()) {
                    it.stock = 0;
                    return true;
                }
            }
            return false;
        }
    
        bool setItemPrice(int sellerIndex, int itemId, double price) {
            if (sellerIndex < 0 || sellerIndex >= (int)sellers.size()) {
                return false;
            }
            for (auto &it : items) {
                if (it.id == itemId && it.sellerId==sellers[sellerIndex].getId()) {
                    it.setPrice(price);
                    return true;
                }
            }
            return false;
        }

        int purchaseItem(int buyerIndex, int itemId, int qty) {
            if (buyerIndex < 0 || buyerIndex >= (int)buyers.size()) {
                return -1;
            }
            Item *itptr = nullptr;
            for (auto &it : items) {
                if (it.id == itemId) {
                    itptr = &it;
                    break;
                }
            }
            if (!itptr) {
                return -1;
            }
            if (itptr->stock < qty) {
                return -1;
            }
            double total = itptr->price * qty;
            if (buyers[buyerIndex].getBalance() < total) {
                return -1;
            }
            int sellerIdx = -1;
            for (int i = 0;i < (int)sellers.size();i++) {
                if (sellers[i].getId() == itptr->sellerId) {
                    sellerIdx = i;
                    break;
                }
            }
            if (sellerIdx == -1) {
                return -1;
            }
            bool w = buyers[buyerIndex].withdraw(total);
            if (!w) {
                return -1;
            }
            sellers[sellerIdx].topUp(total);

            int oid = nextOrderId++;
            orders.emplace_back(oid, buyers[buyerIndex].getId(), sellers[sellerIdx].getId(), itptr->id, qty, total, OrderStatus::PAID);

            itptr->reduceStock(qty);

            buyers[buyerIndex].addOrderId(oid);
            sellers[sellerIdx].addOrderId(oid);
            return oid;
        }

        bool completeOrder(int sellerIndex, int orderId) {
            if (sellerIndex < 0 || sellerIndex >= (int)sellers.size()) {
                return false;
            }
            for (auto &o : orders) {
                if (o.id == orderId && o.sellerId == sellers[sellerIndex].getId() && o.status == OrderStatus::PAID) {
                    o.status = OrderStatus::COMPLETED;
                    return true;
                }
            }
            return false;
        }

        bool cancelOrder(int orderId) {
            for (auto &o : orders) {
                if (o.id == orderId && o.status != OrderStatus::CANCELLED && o.status != OrderStatus::COMPLETED) {
                    if (o.status == OrderStatus::PAID) {
                        int bIdx = -1, sIdx = -1;
                        for (int i = 0; i < (int)buyers.size();i++) {
                            if (buyers[i].getId()== o.buyerId) {
                                bIdx = i;
                            }
                        }
                        for (int i = 0; i < (int)sellers.size();i++) {
                            if (sellers[i].getId()== o.sellerId) {
                                sIdx = i;
                            }
                        }
                        if (bIdx == -1 || sIdx == -1) {
                            return false;
                        }
                        if (!sellers[sIdx].withdraw(o.total)) {
                            return false;
                        }
                        buyers[bIdx].topUp(o.total);
                        for (auto &it : items) {
                            if (it.id == o.ItemId) {
                                it.addStock(o.qty);
                                break;
                            }
                        }
                    }
                    o.status = OrderStatus::CANCELLED;
                    return true;
                }
            }
            return false;
        }

        void listOrdersLastKDays(int k) const {
            cout << "\n===== Orders in the last " << k << " days (approx) =====\n";
            for (const auto &o : orders) {
                o.show();
            }
        }

        vector<Order> listPaidNotCompleted() const {
            vector<Order> out;
            for (const auto &o : orders) {
                if (o.status == OrderStatus::PAID) {
                    out.push_back(o);
                }
            }
            return out;
        }

        vector<pair<int,int>> mostFrequentItems(int m) const {
            map<int,int> freq;
            for (const auto &o : orders) {
                if (o.status != OrderStatus::CANCELLED) {
                    freq[o.ItemId] += o.qty;
                }
            }
            vector<pair<int,int>> vec(freq.begin(), freq.end());
            sort(vec.begin(), vec.end(), [](auto &a, auto &b) {
                return a.second > b.second;
            });
            if ((int)vec.size() > m) {
                vec.resize(m);
            }
            return vec;
        }

        vector<pair<int,int>> mostActiveBuyers(int topk) const {
            map<int,int> cnt;
            for (const auto &o : orders) {
                if (o.status != OrderStatus::CANCELLED) {
                    cnt[o.buyerId]++;
                }
            }
            vector<pair<int,int>> v(cnt.begin(), cnt.end());
            sort(v.begin(), v.end(), [](auto &a, auto &b) {
                return a.second > b.second;
            });
            if ((int)v.size() > topk) {
                v.resize(topk);
            }
            return v;
        }

        vector<pair<int,int>> mostActiveSellers(int topk) const {
            map<int,int> cnt;
            for (const auto &o : orders) {
                if (o.status != OrderStatus::CANCELLED) {
                    cnt[o.sellerId]++;
                }
            }
            vector<pair<int,int>> v(cnt.begin(), cnt.end());
            sort(v.begin(), v.end(), [](auto &a, auto &b) {
                return a.second > b.second;
            });
            if ((int)v.size() > topk) {
                v.resize(topk);
            }
            return v;
        }

        vector<Buyer>& getBuyers() {
            return buyers;
        }
        vector<Seller>& getSellers() {
            return sellers;
        }
        const vector<Item>& getItems() const {
            return items;
        }
        const vector<Order>& getOrders() const {
            return orders;
        }

        Buyer* getBuyerByIndex (int idx) {
            if (idx < 0 || idx >= (int)buyers.size()) {
                return nullptr;
                return &buyers[idx];
            }
        }
        Seller* getSellerByIndex (int idx) {
            if (idx < 0 || idx >= (int)sellers.size()) {
                return nullptr;
                return &sellers[idx];
            }
        }
        Item* getItemById (int itemId) {
            for (auto &it : items) {
                if (it.id == itemId) {
                    return &it;
                }
            }
            return nullptr;
        }
        Order* getOrderById(int orderId) {
            for (auto &o : orders) {
                if (o.id == orderId) {
                    return &o;
                }
            }
            return nullptr;
        }

        Bank& getBank() {
            return bank;
        }

        void saveData() {
            ofstream fout("storedata.txt");
            bank.saveData(fout);
            fout << "[BUYERS]\n";
            for (auto &b : buyers) fout << b.serialize() << endl;
            fout << "[SELLERS]\n";
            for (auto &s : sellers) fout << s.serialize() << endl;
            fout << "[ITEMS]\n";
            for (auto &i : items) fout << i.serialize() << endl;
            fout << "[TRANSACTIONS]\n";
            for (auto &t : orders) fout << t.serialize() << endl;
            fout.close();
        }
    
        void loadData() {
            ifstream fin("storedata.txt");
            if (!fin.is_open()) return;
        
            string line;
            while (getline(fin, line)) {
                if (line.empty()) continue;

                if (line == "[BANK]") {
                    bank.loadData(fin);
                }
                else if (line == "[BUYERS]") {
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        if (line == "[SELLERS]" || line == "[ITEMS]" || line == "[TRANSACTIONS]" || line == "[END]") {
                            fin.seekg(-((int)line.size()) - 1, ios::cur); // mundur 1 baris untuk section berikutnya
                            break;
                        }
                        Buyer b;
                        b.deserialize(line);
                        buyers.push_back(b);
                    }
                }
                else if (line == "[SELLERS]") {
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        if (line == "[ITEMS]" || line == "[TRANSACTIONS]" || line == "[END]") {
                            fin.seekg(-((int)line.size()) - 1, ios::cur);
                            break;
                        }
                        Seller s;
                        s.deserialize(line);
                        sellers.push_back(s);
                    }
                }
                else if (line == "[ITEMS]") {
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        if (line == "[TRANSACTIONS]" || line == "[END]") {
                            fin.seekg(-((int)line.size()) - 1, ios::cur);
                            break;
                        }
                        Item i;
                        i.deserialize(line);
                        items.push_back(i);
                    }
                }
                else if (line == "[TRANSACTIONS]") {
                    while (getline(fin, line)) {
                        if (line.empty()) continue;
                        if (line == "[END]") break;
                        Order t;
                        t.deserialize(line);
                        orders.push_back(t);
                    }
                }
            }
        
            fin.close();
        }
};

#endif