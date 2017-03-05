//
//  Product.hpp
//  C++
//
//  Created by Иван on 04.03.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#ifndef Product_hpp
#define Product_hpp

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Product {
private:
    int amount;
    int price;
    
    string name;
public:
    Product() {
        amount = 0;
        price = 0;
        
        name = "Nothing";
    }
    Product(int amount, int price, string name) {
        this->amount = amount;
        this->price = price;
        
        this->name = name;
    }
    
    void showProductNameAmountPrice() {
        cout << name << ", " << amount << " × " << price << " р." << endl;
    }
    
    void showName() {
        cout << name;
    }
    
    int getAmount() {
        return amount;
    }
    
    int getPrice() {
        return price;
    }
    
    void changeAmount(int newAmount) {
        amount = newAmount;
    }
    
    string getName() {
        return name;
    }
    
};

#endif /* Product_hpp */
