#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "bank.h"
#include "bank_customer.h"
#include "buyer.h"
#include "item.h"
#include "seller.h"
#include "store.h"
#include "transaction.h"

using namespace std;

int main() {
    Store store;
    store.loadData();

    int choice;
    bool running = true;

    while (running) {
        cout << "\nSelect an Option:\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Exit\n";
        cout << "Choose: "; cin >> choice;

        if (choice == 1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string username, password;
            cout << "===== LOGIN =====\n";
            cout << "Enter username: "; getline(cin, username);
            cout << "Enter Password: "; getline(cin, password);

            int bidx = store.loginBuyer(username, password);
            if (bidx != -1) {
                cout << "\nLogged in as Buyer " << store.getBuyers()[bidx].getName() << " (username: " << store.getBuyers()[bidx].getUsername() << ")\n";
                int bChoice;
                while (true) {
                    cout << "\n----- Buyer Menu -----\n";
                    cout << "1. Manage Bank Account\n";
                    cout << "2. Purchase Item\n";
                    cout << "3. List Orders\n";
                    cout << "4. Check Spending (last k days)\n";
                    cout << "5. Upgrade Account to Seller\n";
                    cout << "6. Logout\n";
                    cout << "Choose: "; cin >> bChoice;

                    if (bChoice == 1) {
                        int bank;
                        cout << "\n--- BANK ACCOUNT (BUYER) ---\n";
                        cout << "Balance: " << store.getBuyers()[bidx].getBalance() << endl;
                        cout << "1. Top Up\n";
                        cout << "2. Withdraw\n";
                        cout << "3. List Cash Flow (Today)\n";
                        cout << "4. List Cash Flow (This Month)\n";
                        cout << "5. Logout\n";
                        cout << "Choose: "; cin >> bank;

                        if(bank == 1) {
                            double amt;
                            cout << "Enter amount: "; cin >> amt;
                            store.getBuyers()[bidx].topUp(amt);
                            cout << "Top-up success! \nNew Balance: " << store.getBuyers()[bidx].getBalance() << endl;
                        } else if (bank == 2) {
                            double amt;
                            cout << "Enter amount: "; cin >> amt;
                            if (store.getBuyers()[bidx].withdraw(amt)) {
                                cout << "Withdraw succes! \nNew Balance: " << store.getBuyers()[bidx].getBalance() << endl;
                            } else {
                                cout << "Withdraw failed" << endl;
                            }
                        } else if (bank == 3) {
                            store.getBuyers()[bidx].listCashFlowToday();
                        } else if (bank == 4) {
                            store.getBuyers()[bidx].listCashFlowMonth();
                        } else if (bank == 5){
                            continue;
                        } else {
                            break;
                        }
                    } else if (bChoice == 2) {
                        const vector<Seller> &sellers = store.getSellers();
                        if (sellers.empty()) {
                            cout << "No sellers available.\n";
                            continue;
                        }
                        cout << "Available sellers: " << endl;
                        for (int i = 0; i < (int)sellers.size(); i++) {
                            cout << i << ") " << sellers[i].getStoreName() << "(username: " << sellers[i].getUsername() << ")\n";
                        }
                        int sidx;
                        cout << "Enter seller index: "; cin >> sidx;
                        if (sidx < 0 || sidx >= (int)sellers.size()) {
                            cout << "Invalid seller." << endl;
                            continue;
                        }
                        cout << "Items for seller " << sellers[sidx].getStoreName() << ":\n";
                        for (const auto &it : store.getItems()) {
                            if (it.sellerId == sellers[sidx].getId()) {
                                it.show();
                            }
                        }
                        int iid, qty;
                        cout << "Enter item id to buy: "; cin >> iid;
                        cout << "Enter quantity: "; cin >> qty;
                        int orderId = store.purchaseItem(bidx, iid, qty);
                        if (orderId == -1) {
                            cout << "Purchase failed." << endl;
                        } else {
                            cout << "Purchase successful!. Order id: " << orderId << endl;
                        
                        }
                    } else if (bChoice == 3) {
                        store.getBuyers()[bidx].listMyOrders(store.getOrders());
                    } else if (bChoice == 4) {
                        int k;
                        cout << "Enter k (days) for spending check (approx): "; cin >> k;
                        double spent = store.getBuyers()[bidx].spendingLastKDays(store.getOrders(), k);
                        cout << "Approx spending: " << spent << endl;
                    } else if (bChoice == 5) {
                        Buyer &b = store.getBuyers()[bidx];
                        string sname;
                        cout << "Enter store name for your seller account: "; cin >> sname;
                        string username = b.getUsername();
                        string password = b.getPassword();
                        string phone = b.getPhone();
                        string email = b.getEmail();
                        string addr = b.getAddress();
                        int sid = store.registerSeller(username, password, sname, phone, email, addr, 0.0);
                        if (sid == -1) {
                            string username2 = username + "_s";
                            sid = store.registerSeller(username2, password, sname, phone, email, addr, 0.0);
                            if (sid == -1) {
                                cout << "Upgrade failed." << endl;
                            } else {
                                cout << "Upgraded: seller username: " << username2 << " id: " << sid << endl;
                            }
                        } else {
                            cout << "Upgraded: seller username: " << username << " id: " << sid << endl;
                        }
                    } else if (bChoice == 6) {
                        cout << "Logging out..." << endl;
                        break;
                    } else {
                        cout << "Invalid choice." << endl;
                    }
                }     
                
             } else {
                 int sidx = store.loginSeller(username, password);
                 if (sidx != -1) {
                     cout << "\nLogged in as Seller " << store.getSellers()[sidx].getStoreName() << " (username: " << store.getSellers()[sidx].getUsername() << ")\n";
                     int sChoice;
                     while (true) {
                         cout << "\n----- Seller Menu -----\n";
                         cout << "1. All Buyer Features\n";
                         cout << "2. Manage Inventory\n";
                         cout << "3. View Top k Most popular Items (Monthly)\n";
                         cout << "4. View Loyal customers (Monthly)\n";
                         cout << "5. List Orders (for your shop)\n";
                         cout << "6. Store Reports\n";
                         cout << "7. Logout\n";
                         cout << "Choose: "; cin >> sChoice;

                         if (sChoice == 1) {
                             int sbank;
                             cout << "\n--- BANK ACCOUNT (SELLER) ---\n";
                             cout << "Balance: " << store.getSellers()[sidx].getBalance() << endl;
                             cout << "1. Top Up\n";
                             cout << "2. Withdraw\n";
                             cout << "3. List Cash Flow (Today)\n";
                             cout << "4. List Cash Flow (This Month)\n";
                             cout << "5. Logout\n";
                             cout << "Choose: "; cin >> sbank;

                             if(sbank == 1) {
                                 double amt;
                                 cout << "Enter amount: "; cin >> amt;
                                 store.getSellers()[sidx].topUp(amt);
                                 cout << "Top-up success!" << endl;
                             } else if (sbank == 2) {
                                 double amt;
                                 cout << "Enter amount: "; cin >> amt;
                                 if (store.getSellers()[sidx].withdraw(amt)) {
                                     cout << "Withdrawn." << endl;
                                 } else { cout << "Withdraw failed." << endl;}
                             } else if (sbank == 3) {
                                 store.getSellers()[sidx].listCashFlowToday();
                             } else if (sbank == 4) {
                                 store.getSellers()[sidx].listCashFlowMonth();
                             } else if (sbank == 5) {
                                 continue;
                             } else {
                                 break;
                             }
                         } else if (sChoice == 2) {
                             int sinv;
                             cout << "\n--- MANAGE INVENTORY ---\n";
                             cout << "1. Register New Item\n";
                             cout << "2. Replenish Stock\n";
                             cout << "3. Discard Item\n";
                             cout << "4. Set Price\n";
                             cout << "5. List My Items\n";
                             cout << "6. Back\n";
                             cout << "Choose: "; cin >> sinv;
                             if (sinv == 1) {
                                 string in, id;
                                 int is;
                                 double price;
                                 cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                 cout << "Item Name: "; getline(cin, in);
                                 cout << "Description: "; getline(cin, id);
                                 cout << "Price: "; cin >> price;
                                 cout << "Initial Stock: "; cin >> is;
                                 int iid = store.createItem(sidx, in, id, price, is);
                                 if (iid == -1) {
                                     cout << "Failed to Create Item." << endl;
                                 } else {
                                     cout << "Item created with id: " << iid << endl;
                                 }
                             } else if (sinv == 2) {
                                 int iid, q;
                                 cout << "Item id to replenish: "; cin >> iid;
                                 cout << "Quantity: "; cin >> q;
                                 if (store.replenishItem(sidx, iid, q)) {
                                     cout << "Replenished." << endl;
                                 } else { cout << "Failed to replenish." << endl;}
                             } else if (sinv == 3) {
                                 int iid;
                                 cout << "Item id to discard: "; cin >> iid;
                                 if (store.discardItem(sidx, iid)) {
                                     cout << "Discarded." << endl;
                                 } else { cout << "Failed to discard." << endl; }
                             } else if (sinv == 4) {
                                 int iid;
                                 double price;
                                 cout << "Item id to set price: "; cin >> iid;
                                 cout << "Price: "; cin >> price;
                                 if (store.setItemPrice(sidx, iid, price)) {
                                     cout << "Set Price Success." << endl;
                                 } else { cout << "Failed." << endl; }
                             } else if (sinv == 5) {
                                 store.getSellers()[sidx].listMyItems(store.getItems());
                             } else if (sinv == 6) {
                                 break;
                             } else {
                                 "Invalid choice.\n";
                             }
                         } else if (sChoice == 3) {
                             int k;
                             cout << "Enter k (top items): "; cin >> k;
                             auto top = store.mostFrequentItems(k);
                             cout << "\nTop Items (by qty sold):\n";
                             for (auto &p : top) {
                                 Item* it = store.getItemById(p.first);
                                 if (it) {
                                     cout << "Item Id: " << it->id << " name: " << it->name << " sold: " << p.second << endl;
                                 }
                             }
                         } else if (sChoice == 4) {
                             int k;
                             cout << "Enter k (top k loyal customers): "; cin >> k;
                             int sellerRealId = store.getSellers()[sidx].getId();
                             std::map<int,int> freq;
                             for (const auto &o : store.getOrders()) {
                                 if (o.sellerId == sellerRealId && o.status != OrderStatus::CANCELLED) {
                                 freq[o.buyerId]++;
                                 }
                             }
                             vector<std::pair<int,int>> vec(freq.begin(), freq.end());
                             std::sort(vec.begin(), vec.end(), [](auto &a, auto &b) { return a.second > b.second; });
                             cout << "\nTop Loyal Customers:\n";
                             for (int i = 0;i < k && i < (int)vec.size();i++) {
                                 cout << "Buyer Id: " << vec[i].first << " orders: " << vec[i].second << endl;
                             }
                         } else if (sChoice == 5) {
                             store.getSellers()[sidx].listMyOrders(store.getOrders());
                         } else if (sChoice == 6) {
                                 int str;
                                     cout << "\n--- STORE REPORTS---\n";
                                     cout << "1. List All transactions of the Latest k days\n";
                                     cout << "2. List All Paid transactions but yet to be Completed\n";
                                     cout << "3. List Most m Frequent Item transactions\n";
                                     cout << "4. List Most Active buyers (by transactions)\n";
                                     cout << "5. List Most Active Sellers (by transactions)\n";
                                     cout << "6. Back\n";
                                     cout << "Choose: "; cin >> str;
    
                                     if (str == 1) {
                                         int k;
                                         cout << "k days: "; cin >> k;
                                         store.listOrdersLastKDays(k);
                                     } else if (str == 2) {
                                     auto v = store.listPaidNotCompleted();
                                     cout << "\nPaid but not completed:\n";
                                     for (auto &o : v) {
                                         o.show();
                                         }
                                     } else if (str == 3) {
                                         int m;
                                         cout << "m: "; cin >> m;
                                         auto top = store.mostFrequentItems(m);
                                         for (auto &p : top) {
                                             Item* it = store.getItemById(p.first);
                                             if (it) {
                                             cout << "Item id: " << it->id << " name: " << it->name << " quantity: " << p.second << endl;
                                             }
                                         }
                                     } else if (str == 4) {
                                         int k;
                                         cout << "Top k: "; cin >> k;
                                         auto v = store.mostActiveBuyers(k);
                                         cout << "\nMost Active Buyers:\n";
                                         for (auto &p : v) {
                                             cout << "Buyer id: " << p.first << " tx Count: " << p.second << endl;
                                         }
                                     } else if (str == 5) {
                                         int k;
                                         cout << "Top k: "; cin >> k;
                                         auto v = store.mostActiveSellers(k);
                                         cout << "\nMost Active Sellers:\n";
                                         for (auto &p : v) {
                                             cout << "Seller id: " << p.first << " tx Count: " << p.second << endl;
                                         }
                                     } else if (str == 6) {
                                         continue;
                                     } else { "Invalid choice.\n";}
                                } else if (sChoice == 7) {
                                 break;
                             } else {
                             "Invalid Choice.\n";
                            }
                        }
                } else {
                    cout << "Invalid Credentials (no such buyer or seller)." << endl;
                }
            }
        } else if (choice == 3) {
            store.saveData();
            cout << "Exiting." << endl;
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }
    return 0;
}