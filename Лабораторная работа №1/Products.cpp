//
//  Products.cpp
//  C++
//
//  Created by Иван on 01.03.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#include <stdio.h>
#include <iostream>

using namespace std;

class Product {
private:
    int amountOfThisInVendingMachine;
    int priceOfThis;
    
    string name;
public:
    Product() {
        amountOfThisInVendingMachine = 0;
        priceOfThis = 0;
        
        name = "Nothing";
    }
    Product(int amount, int price, string name) {
        amountOfThisInVendingMachine = amount;
        priceOfThis = price;
        
        this->name = name;
    }
    
    void BuyThis(int &money) {
        int amountToBuy;
        
        cout << "У Вас осталось " << money << " рублей." << endl;
        
        while (1) {
            cout << "Хотите ли Вы купить " << name << " (y/n)?" << endl;
            char c;
            cin >> c;
            if (c == 'y') {
                break;
            }
            else {
                return;
            }
        }
        
        cout << "Введите количество " << name << ":" << endl;
        while (1) {
            cin >> amountToBuy;
            if (amountToBuy <= 0) {
                cout << "Повторите ввод!" << endl;
            }
            else {
                break;
            }
        }
        
        if(amountToBuy > amountOfThisInVendingMachine) {
            cout << "Извините, у нас только " << amountOfThisInVendingMachine << " единиц товара "  << name << "." << endl;
            cout << "Мы Вам выдадим оставшееся." << endl;
        }
        
        if(amountToBuy * priceOfThis > money) {
            cout << "У Вас не хватает денег :(" << endl;
            int n = money / priceOfThis;
            cout << "Вы можете купить только " << n << " единиц товара " << name << endl << "Желаете ли купить такое количество товара (y/n)?" << endl;
            
            char c;
            cin >> c;
            if (c == 'y') {
                amountToBuy = n;
                money -= amountToBuy * priceOfThis;
                amountOfThisInVendingMachine -= amountToBuy;
                return;
            }
            else {
                return;
            }
        }
        
        money -= amountToBuy * priceOfThis;
        amountOfThisInVendingMachine -= amountToBuy;
        
    }
    
};
