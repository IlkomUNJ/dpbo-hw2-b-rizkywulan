#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

enum class OrderStatus{CREATED, PAID, COMPLETED, CANCELLED};

inline std::string orderStatusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::PAID: return "PAID";
        case OrderStatus::COMPLETED: return "COMPLETED";
        case OrderStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

inline OrderStatus stringToOrderStatus(const std::string &s) {
    if (s == "PAID") return OrderStatus::PAID;
    if (s == "COMPLETED") return OrderStatus::COMPLETED;
    if (s == "CANCELLED") return OrderStatus::CANCELLED;
    return OrderStatus::PAID;
}

struct Order {
    int id;
    int buyerId;
    int sellerId;
    int ItemId;
    int qty;
    double total;
    OrderStatus status = OrderStatus::CREATED;
    string when;

        Order() = default;
        Order(int id_, int b, int s, int item, int q, double tot, OrderStatus st)
        : id(id_), buyerId(b), sellerId(s), ItemId(item), qty(q), total(tot), status(st) {
            std::time_t now = std::time(nullptr);
            char buf[64];
            std::strftime(buf, sizeof(buf), "%b %d %H:%M:%S %Y", std::localtime(&now));
            when = string(buf);
        }

    void show() const {
        cout << "OrderId = " << id << " | BuyerId = " << buyerId << " | SellerId = " 
        << sellerId << " | ItemId = " << ItemId << " | Quantity = " << qty << " | Total = "
        << total << " | Status = " << (status==OrderStatus::CREATED ? "CREATED" 
                                    : status==OrderStatus::PAID ? "PAID" 
                                    : status==OrderStatus::COMPLETED ? "COMPLETED" : "CANCELLED")
        << " | When = " << when << endl;
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << buyerId << "|"
            << sellerId << "|"
            << ItemId << "|"
            << total << "|"
            << orderStatusToString(status) << "|"
            << when;
        return oss.str();
    }

    void deserialize(const std::string &line) {
        std::istringstream iss(line);
        std::string token;
        std::getline(iss, token, '|'); buyerId = std::stoi(token);
        std::getline(iss, token, '|'); sellerId = std::stoi(token);
        std::getline(iss, token, '|'); ItemId = std::stoi(token);
        std::getline(iss, token, '|'); total = std::stod(token);
        std::getline(iss, token, '|'); status = stringToOrderStatus(token);
        std::getline(iss, when, '|');
    }

    void print() const {
        std::cout << "Buyer ID: " << buyerId
                  << ", Seller ID: " << sellerId
                  << ", Item ID: " << ItemId
                  << ", Total: " << total
                  << ", Status: " << orderStatusToString(status)
                  << ", Date: " << when << std::endl;
    }
};

#endif