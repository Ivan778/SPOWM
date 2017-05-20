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
            string name;
            cout << "Введите название файла, который вы хотите удалить:" << endl;
            cin >> name;
            
            string ext;
            cout << "Введите расширение файла, который вы хотите удалить:" << endl;
            cin >> ext;
            w.deleteModule(toGo, file, name, ext);
            continue;
            
        }
        
        // Удалить пустую директорию
        if (cmd == "rmdir") {
            string name;
            cout << "Введите название директории, которую вы хотите удалить:" << endl;
            cin >> name;
            
            w.deleteModule(toGo, directory, name, "");
            continue;
            
        }
        
        // Удалить директорию с содержимым
        if (cmd == "rmdwc") {
            cout << "Введите имя директории, которую надо удалить: " << endl;
            string toDelete;
            cin >> toDelete;
            
            w.deleteDirectoryWithContent(toGo, toDelete);
            continue;
            
        }
        
        // Выйти из программы
        if (cmd == "quit") {
            w.quit();
            break;
            
        }
        
        if (cmd == "gfs") {
            cout << w.getFileSize() << endl;
        }
        
    }
    
    return 0;
}
