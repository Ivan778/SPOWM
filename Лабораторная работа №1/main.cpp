//
//  main.cpp
//  C++
//
//  Created by Иван on 01.07.16.
//  Copyright © 2016 IvanCode. All rights reserved.
//

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "Products.cpp"

using namespace std;

void askManToPutMoney(int file, int &money) {
    int amountToPut;
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
    
    FILE *stream;
    stream = fdopen (file, "w");
    fprintf (stream, "%d", amountToPut);
    fclose (stream);
}

void showAmountOfMoneyInVendingMachine(int file, int &money) {
    int m = 0;
    FILE *stream;
    stream = fdopen (file, "r");
    
    fscanf(stream, "%d", &m);
    
    fclose (stream);
    
    cout << "Вы положили " << m << " рублей." << endl;
    
    money -= m;
}

void chooseProduct(int file, vector<Product> &products) {
    int money;
    
    FILE *stream;
    
    stream = fdopen (file, "r");
    fscanf(stream, "%d", &money);
    
    fclose (stream);
    
    while (1) {
        for (int i = 0; i < products.size(); i++) {
            products[i].BuyThis(money);
        }
        cout << "Хотите ли вы продолжить покупки (y/n)?" << endl;
        
        char c;
        cin >> c;
        if (c != 'y') {
            return;
        }
    }
}

int main(void) {
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
    
    int money = 15;
    int myPipe[2];
    
    if (pipe (myPipe)) {
        fprintf (stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }
    
    pid_t process;
    process = fork();
    if (process == 0) {
        //Мы в дочернем процессе
        close(myPipe[0]);
        askManToPutMoney(myPipe[1], money);
        
        
        
        
        return EXIT_SUCCESS;
    }
    else if (process < 0) {
        fprintf (stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    }
    else {
        //Родительский процесс
        wait(NULL);
        close(myPipe[1]);
        
        chooseProduct(myPipe[0], products);
        
        
        
        return  EXIT_SUCCESS;
    }
    
    return 0;
}
