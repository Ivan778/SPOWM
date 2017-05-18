//
//  main.cpp
//  SPO7
//
//  Created by Иван on 07.05.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "Writer.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    Writer w;
    
    int toGo = sizeof(int);
    string path = "~/";
    
    while(1) {
        cout << path << ": ";
        
        string cmd;
        cin >> cmd;
        
        // Перейти в директорию
        if (cmd == "cd") {
            toGo = w.changeDirectory(toGo, path);
            continue;
        }
        
        // Создать папку
        if (cmd == "mkdir") {
            w.writeToDirectory(toGo, directory);
            continue;
        }
        
        // Создать файл
        if (cmd == "touch") {
            w.writeToDirectory(toGo, file);
            continue;
        }
        
        // Перейти в предыдущую директорию
        if (cmd == "..") {
            toGo = w.goToPreviousDirectory(toGo, path);
            continue;
        }
        
        // Показать содержимое текущей директории
        if (cmd == "ls") {
            w.showContent(toGo);
            continue;
        }
        
        // Форматировать диск
        if (cmd == "dfmt") {
            cout << "Вы уверены, что хотите отформатировать диск (y/n)? Восстановить содержимое будет уже невозможно." << endl;
            cin >> cmd;
            
            if (cmd == "y") {
                w.diskFormatting();
            }
            
            continue;
        }
        
        // Показать занятое место на диске
        if (cmd == "dsku") {
            w.showAmountOfUsedMemory();
            continue;
        }
        
        // Открыть файл
        if (cmd == "open") {
            w.showFileContent(toGo);
            continue;
        }
        
        // Удалить файл
        if (cmd == "rm") {
            w.deleteModule(toGo, file);
            continue;
        }
        
        // Выйти из программы
        if (cmd == "quit") {
            w.quit();
            break;
        }
        
    }
    
    return 0;
}
