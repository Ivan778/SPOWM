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
#include <vector>
#include <fstream>
#include "Product.hpp"
#include "CreateFiles.hpp"

using namespace std;

MySpace s;

int main(void) {
    //Создаём файлы сохранения, если они ещё не созданы
    cout << "Вы хотите создать файлы с деньгами и продуктами (y/n)?" << endl;
    char c;
    cin >> c;
    if (c == 'y') {
        s.createFiles();
    }
    
    //А здесь уже идёт работа с процессами
    pid_t process;
    process = fork();
    if (process == 0) {
        //Мы в дочернем процессе
        s.askManToPutMoney();
        
        return EXIT_SUCCESS;
    }
    else if (process < 0) {
        fprintf (stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    }
    //Родительский процесс
    waitpid(process, NULL, NULL);
    s.showAmountOfMoneyInVendingMachine();
    s.showProducts();
    
    process = fork();
        
    if (process == 0) {
        //Мы опять в дочернем процессе, только уже в новом дочернем
        s.askManToEnterProducts();
        
        return EXIT_SUCCESS;
    }
    else if (process < 0) {
        fprintf (stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    }
    waitpid(process, NULL, NULL);
    s.performOrder();
    
    process = fork();
    
    if (process == 0) {
        //Мы опять в дочернем процессе, но здесь мы выведем информацию о продуктах, которые были успешно куплены, а также оставшиеся деньги
        s.showPurchases();
        
        return EXIT_SUCCESS;
    }
    else if (process < 0) {
        fprintf (stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    }
    
    
    return 0;
}
