//
//  CreateFiles.hpp
//  C++
//
//  Created by Иван on 04.03.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#ifndef CreateFiles_hpp
#define CreateFiles_hpp

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "Product.hpp"

using namespace std;

class MySpace {
public:
    void createFiles() {
        Product cola(10, 2, "Кока Кола");
        Product cashew(6, 4, "Кешью");
        Product water(12, 1, "Вода");
        Product snickers(8, 2, "Сникерс");
        Product fanta(9, 2, "Фанта");
        
        vector<Product> products;
        products.push_back(cola);
        products.push_back(cashew);
        products.push_back(water);
        products.push_back(snickers);
        products.push_back(fanta);
        
        fstream productsF;
        productsF.open("CP.txt", ios::trunc | ios::in | ios::out | ios::binary);
        if(!productsF.is_open()) {
            cout << "Файл не открыт (choose product)!" << endl;
            return;
        }
        
        productsF.seekp(0,ios_base::beg);
        for (int i = 0; i < products.size(); i++) {
            productsF.write(reinterpret_cast<char*>(&products[i]),sizeof(Product));
        }
        
        productsF.close();
        
        fstream moneyBill;
        moneyBill.open("Bill.txt", ios::trunc | ios::in | ios::out | ios::binary);
        if(!moneyBill.is_open()) {
            cout << "Файл не открыт (bill)!" << endl;
            return;
        }
        
        int money = 20;
        
        moneyBill.seekp(0, ios_base::beg);
        moneyBill.write(reinterpret_cast<char*>(&money),sizeof(int));
        
        moneyBill.close();
    }
    
    void askManToPutMoney() {
        int amountToPut, money;
        
        fstream moneyBill;
        moneyBill.open("Bill.txt", ios::in | ios::out | ios::binary);
        if(!moneyBill.is_open()) {
            cout << "Файл не открыт (bill)!" << endl;
            return;
        }
        
        moneyBill.seekg(0, ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&money),sizeof(int));
        
        cout << "У Вас всего " << money << " рублей." << endl;
        cout << "Положите немного денег (например, 5 рублей):" << endl;
        while (1) {
            cin >> amountToPut;
            if (amountToPut > money) {
                cout << "У Вас не хватает денег на счёте. Ваш счёт - " << money << " рублей. Повторите снова:" << endl;
            }
            else {
                money -= amountToPut;
                break;
            }
        }
        
        moneyBill.seekp(0, ios_base::beg);
        moneyBill.write(reinterpret_cast<char*>(&money),sizeof(int));
        
        moneyBill.seekp(0 + sizeof(int), ios_base::beg);
        moneyBill.write(reinterpret_cast<char*>(&amountToPut),sizeof(int));
        
        moneyBill.close();
        
    }
    
    void showAmountOfMoneyInVendingMachine() {
        int amountToPut, money;
        
        fstream moneyBill;
        moneyBill.open("Bill.txt", ios::in | ios::out | ios::binary);
        if(!moneyBill.is_open()) {
            cout << "Файл не открыт (bill)!" << endl;
            return;
        }
        
        moneyBill.seekg(0, ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&money),sizeof(int));
        
        moneyBill.seekg(0 + sizeof(int), ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&amountToPut),sizeof(int));
        
        cout << "Вы положили " << amountToPut << " рублей." << endl;
    }
    
    void showProducts() {
        cout << endl << "Сегодня у нас в продаже: " << endl;
        
        fstream productsF;
        productsF.open("CP.txt", ios::in | ios::out | ios::binary);
        if(!productsF.is_open()) {
            cout << "Файл не открыт (choose product)!" << endl;
            return;
        }
        
        Product p;
        
        productsF.seekg(0, ios_base::end);
        
        int s = (int)productsF.tellg() / sizeof(Product);
        
        
        for (int i = 0; i < s; i++) {
            productsF.seekg(sizeof(Product) * i, ios_base::beg);
            productsF.read(reinterpret_cast<char*>(&p),sizeof(Product));
            cout << i << ") ";
            p.showProductNameAmountPrice();
        }
        
        productsF.close();
    }
    
    void askManToEnterProducts() {
        fstream order;
        order.open("Order.txt", ios::trunc | ios::in | ios::out | ios::binary);
        if(!order.is_open()) {
            cout << "Файл не открыт (order)!" << endl;
            return;
        }
        order.seekp(0, ios_base::beg);
        int am;
        
        fstream productsF;
        productsF.open("CP.txt", ios::in | ios::out | ios::binary);
        if(!productsF.is_open()) {
            cout << "Файл не открыт (choose product)!" << endl;
            return;
        }
        
        Product p;
        
        productsF.seekg(0, ios_base::end);
        
        int s = (int)productsF.tellg() / sizeof(Product);
        
        cout << endl << "Заказывайте: " << endl;
        
        for (int i = 0; i < s; i++) {
            productsF.seekg(sizeof(Product) * i, ios_base::beg);
            productsF.read(reinterpret_cast<char*>(&p),sizeof(Product));
            cout << "Введите количество товара ";
            p.showName();
            cout << ", которое вы хотите купить: ";
            cin >> am;
            if (am < 0) {
                cout << "Умный типо. И я тоже не глуп. Пусть количество будет равняться нулю." << endl;
                am = 0;
            }
            order.seekp(i * sizeof(int), ios_base::beg);
            order.write(reinterpret_cast<char*>(&am),sizeof(int));
        }
        
        productsF.close();
        order.close();
    }
    
    void performOrder() {
        fstream order;
        order.open("Order.txt", ios::in | ios::out | ios::binary);
        if(!order.is_open()) {
            cout << "Файл не открыт (order)!" << endl;
            return;
        }
        
        order.seekg(0, ios_base::end);
        int amount = (int)order.tellg() / sizeof(int);
        int n;
        
        fstream productsF;
        productsF.open("CP.txt", ios::in | ios::out | ios::binary);
        if(!productsF.is_open()) {
            cout << "Файл не открыт (choose product)!" << endl;
            return;
        }
        
        Product p;
        
        int amountToPut, money;
        
        fstream moneyBill;
        moneyBill.open("Bill.txt", ios::in | ios::out | ios::binary);
        if(!moneyBill.is_open()) {
            cout << "Файл не открыт (bill)!" << endl;
            return;
        }
        
        moneyBill.seekg(0, ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&money),sizeof(int));
        
        moneyBill.seekg(0 + sizeof(int), ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&amountToPut),sizeof(int));
        
        fstream manBought;
        manBought.open("ManBought.txt", ios::trunc | ios::in | ios::out | ios::binary);
        if(!manBought.is_open()) {
            cout << "Файл не открыт (order)!" << endl;
            return;
        }
        manBought.seekp(0, ios_base::beg);
        
        int mBi = 0;
        
        for (int i = 0; i < amount; i++) {
            productsF.seekg(sizeof(Product) * i, ios_base::beg);
            productsF.read(reinterpret_cast<char*>(&p),sizeof(Product));
            
            order.seekg(i * sizeof(int), ios_base::beg);
            order.read(reinterpret_cast<char*>(&n),sizeof(int));
            
            if (n == 0) {
                continue;
            }
            
            if (n > p.getAmount()) {
                cout << "В наличии только " << p.getAmount() << " ";
                p.showName();
                cout << endl << "Если Вы положили на счёт достаточно наличности, то я Вам выдам оставшееся." << endl;
                n = p.getAmount();
            }
            
            if (n * p.getPrice() > amountToPut) {
                cout << "Увы, у Вас не хватает " << (n * p.getPrice() - amountToPut) << " рублей." << endl;
                n = amountToPut / p.getPrice();
                cout << "Я Вам выдам " << n << " ";
                p.showName();
                cout << "." << endl;
            }
            
            amountToPut -= n * p.getPrice();
            p.changeAmount(p.getAmount() - n);
            
            productsF.seekp(sizeof(Product) * i, ios_base::beg);
            productsF.write(reinterpret_cast<char*>(&p),sizeof(Product));
            
            Product inBF(n, p.getPrice(), p.getName());
            manBought.seekp(sizeof(Product) * mBi++, ios_base::beg);
            manBought.write(reinterpret_cast<char*>(&inBF), sizeof(Product));
        }
        
        //Занесли изменения в файл с деньгами
        if (amountToPut > 0) {
            money += amountToPut;
            moneyBill.seekp(0, ios_base::beg);
            moneyBill.write(reinterpret_cast<char*>(&money),sizeof(int));
            
            amountToPut = 0;
            moneyBill.seekp(sizeof(int), ios_base::beg);
            moneyBill.write(reinterpret_cast<char*>(&amountToPut),sizeof(int));
            
            moneyBill.close();
        }
        
        productsF.close();
        order.close();
        manBought.close();
    }
    
    void showPurchases() {
        fstream manBought;
        manBought.open("ManBought.txt", ios::in | ios::out | ios::binary);
        if(!manBought.is_open()) {
            cout << "Файл не открыт (order)!" << endl;
            return;
        }
        manBought.seekg(0, ios_base::end);
        int size = (int)manBought.tellg() / sizeof(Product);
        
        fstream moneyBill;
        moneyBill.open("Bill.txt", ios::in | ios::out | ios::binary);
        if(!moneyBill.is_open()) {
            cout << "Файл не открыт (bill)!" << endl;
            return;
        }
        int money;
        
        moneyBill.seekg(0, ios_base::beg);
        moneyBill.read(reinterpret_cast<char*>(&money),sizeof(int));
        
        cout << "Вы купили: " << endl;
        for (int i = 0; i < size; i++) {
            manBought.seekg(i * sizeof(Product), ios_base::beg);
            Product p;
            manBought.read(reinterpret_cast<char*>(&p), sizeof(Product));
            
            cout << p.getAmount() << " ";
            p.showName();
            cout << endl;
        }
        
        cout << "У Вас осталось " << money << " р." << endl;
        
        manBought.close();
        moneyBill.close();
    }
};

#endif /* CreateFiles_hpp */
