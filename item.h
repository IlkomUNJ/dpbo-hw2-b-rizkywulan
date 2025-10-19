#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct Item {
    int id;
    string description;
    string name;
    double price;
    int stock;
    int soldCount;
    int sellerId;

    Item() = default;
    Item(int id_, const string &n, const string &desc, double p, int s, int seller)
        : id(id_), name(n), description(desc), price(p), stock(s), soldCount(0), sellerId(seller) {}

    void addStock(int q) {
        if (q > 0) {
            stock += q;
        }
    }

    bool reduceStock(int q) {
        if (q > stock) {
            return false;
        }
        stock -= q;
        soldCount += q;
        return true;
    }
    
    void setPrice(double p) {
        if (p >= 0) {
            price = p;
        }
    }

    void show() const {
        cout << "ItemId = " << id << " | " << name << " | " << description <<
        " | Price: " << price << " | Stock: " << stock << " | Sold: " << soldCount << endl;
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << id << "|" << sellerId << "|" << name << "|" << price << "|" << stock;
        return oss.str();
    }
    
    void deserialize(const std::string &line) {
        std::istringstream iss(line);
        std::string token;
    
        // Nama item
        std::getline(iss, name, '|');
    
        // Harga (double)
        std::getline(iss, token, '|');
        if (!token.empty()) {
            try { price = std::stod(token); }
            catch (...) { price = 0.0; }
        } else {
            price = 0.0;
        }
    
        // Stok (int)
        std::getline(iss, token, '|');
        if (!token.empty()) {
            try { stock = std::stoi(token); }
            catch (...) { stock = 0; }
        } else {
            stock = 0;
        }
    }
};

#endif