//
//  Writer.hpp
//  SPO7
//
//  Created by Иван on 17.05.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#ifndef Writer_hpp
#define Writer_hpp

#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

// Вместимость хранилища
const int STORAGE_SIZE = 8000;

// Типы модулей
enum moduleType { file, directory };
enum createMode { newFileMode, renameFileMode };

// Структура, которая описывает каждый элемент диска
struct Module {
    // Тип модуля (папка или файл)
    moduleType type;
    // Имя модуля
    char name[20];
    // Расширение модуля (в случае файла)
    char extension[5];
    // Позиция, на которую указывает модуль (только для папки: указывает на индекс, с которого можно читать содержимое)
    int position;
    // Размер файла (количество символов в нём)
    int fileSize;
    
    Module() {
        position = 0;
        fileSize = 0;
    }
    
};

// Структура, которая описывает модуль, хранящий количество элементов в директории и индекс предыдущей папки
struct AmountAndPreviousPosition {
    int amount;
    int previousPosition;
};

class Writer {
private:
    fstream stream;
    char diskName[9];
public:
    // Открытие файла происходит при создании экземпляра конструктора
    Writer() {
        strcpy(this->diskName, "Disk.txt");
        // Открываем файл в бинарном режиме для чтения/записи
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
        
    }
    
    void reopenFile() {
        // Открываем файл в бинарном режиме для чтения/записи
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
    }
    
    void quit() {
        stream.close();
        cout << "Досвидания!" << endl;
    }
    
    // Форматирует диск (просто записываем в начало диска структуру типа АmountAndPreviousPosition, которая говорит, что в корне ничего не записано, т.е. количество файлов и папок равно нулю)
    void diskFormatting() {
        AmountAndPreviousPosition firstWrite;
        firstWrite.amount = 0;
        firstWrite.previousPosition = sizeof(int);;
        
        stream.close();
        // Открываем файл в бинарном режиме для чтения/записи и чистим его
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary | ios::trunc);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
        
        int usedBytes = 0;
        
        // Записали в начало файла количество занятой информации (по началу это ноль)
        stream.seekp(0, ios::beg);
        stream.write(reinterpret_cast<char*>(&usedBytes), sizeof(int));
        
        // Записываем в корень в файл
        stream.seekp(sizeof(int), ios::beg);
        stream.write(reinterpret_cast<char*>(&firstWrite), sizeof(AmountAndPreviousPosition));
        
    }
    
    // Узнаёт размер файла
    int getFileSize() {
        this->stream.seekp(0, ios::end);
        return (int)(this->stream.tellp());
    }
    
    Module createDirectory(int position) {
        Module m;
        
        cout << "Введите название новой директории (до 20 символов): " << endl;
        string str;
        cin >> str;
        
        for (int i = 0; i < str.length(); i++) {
            m.name[i] = str[i];
        }
        m.name[19] = '\0';
        
        for (int i = (int)str.length(); i < 20; i++) {
            m.name[i] = '\0';
        }
        
        m.type = directory;
        m.position = position;
        
        return m;
    }
    
    Module createFile(createMode mode) {
        Module m;
        
        if (mode == newFileMode) {
            cout << "Введите название нового файла (до 20 символов): " << endl;
        } else {
            cout << "Введите новое имя файла (до 20 символов): " << endl;
        }
        
        string str;
        cin >> str;
        
        for (int i = 0; i < 19; i++) {
            m.name[i] = str[i];
        }
        m.name[19] = '\0';
        
        for (int i = (int)str.length(); i < 19; i++) {
            m.name[i] = '\0';
        }
        
        if (mode == newFileMode) {
            cout << "Введите расширение нового файла (до 20 символов): " << endl;
        } else {
            cout << "Введите новое расширение файла (до 20 символов): " << endl;
        }
        cin >> str;
        
        for (int i = 0; i < 4; i++) {
            m.extension[i] = str[i];
        }
        
        m.extension[4] = '\0';
        
        for (int i = (int)str.length(); i < 4; i++) {
            m.extension[i] = '\0';
        }
        
        m.type = file;
        
        return m;
    }
    
    void showModule(Module m) {
        if (m.type == file) {
            cout << m.name << "." << m.extension << " (" << m.fileSize << "/" << m.fileSize + sizeof(Module) << " байт)" << " - файл" << endl;
        } else {
            cout << m.name << " - папка" << endl;
        }
    }
    
    void showContent(int beginning) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        if (pAa.amount == 0) {
            cout << "Пустая директория!" << endl;
            return;
        }
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        Module m;
        
        int goThrough = beginning - sizeof(Module);
        
        for (int i = 0; i < pAa.amount; i++) {
            // Вычислили место, куда нужно поставить курсор
            goThrough += sizeof(Module);
            
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough, ios::beg);
            // Считали модуль из файла
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            // Вывели его на экран
            showModule(m);
            
            if (m.type == file) {
                // Перешли на количество символов в файле
                goThrough += m.fileSize;
            }
        }
        
        
    }
    
    // Переходим в другую директорию по имени
    int changeDirectory(int beginning, string &path) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        string goTo;
        cout << "Введите название директории, в которую хотите перейти:" << endl;
        cin >> goTo;
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль из файла
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == directory) {
                if (strcmp(m.name, goTo.c_str()) == 0) {
                    // Если стоим в корне
                    if (path[path.length() - 1] == '/') {
                        path += goTo;
                    } else {
                        path += "/" + goTo;
                    }
                    
                    // Вернули позицию директории, в которую нужно перейти
                    return m.position;
                }
            }
            
            if (m.type == file) {
                goThrough += m.fileSize;
            }
            
        }
        
        cout << "Такой папки в текущей директории нет!" << endl;
        return beginning - sizeof(AmountAndPreviousPosition);
        
    }
    
    // Осуществляет сдвиг всех данных в файле вправо на размер одного char начиная с указанной точки
    void shiftRightOnChar(int beginning) {
        for (int i = getFileSize(); i > beginning; i--) {
            char c;
            stream.seekg(i - 1, ios::beg);
            stream.read(reinterpret_cast<char*>(&c), sizeof(char));
            
            stream.seekp(i, ios::beg);
            stream.write(reinterpret_cast<char*>(&c), sizeof(char));
        }
    }
    
    // Осуществляет сдвиг всех данных в файле влево на размер одного char начиная с указанной точки
    void shiftLeftOnChar(int beginning, int offset) {
        for (int i = beginning + offset; i < getFileSize(); i++) {
            char c;
            stream.seekg(i, ios::beg);
            stream.read(reinterpret_cast<char*>(&c), sizeof(char));
            
            stream.seekp(i - offset, ios::beg);
            stream.write(reinterpret_cast<char*>(&c), sizeof(char));
        }
    }
    
    // Позволяет осуществлять переход в предыдущую директорию
    int goToPreviousDirectory(int beginning, string &path) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Формируем путь
        string c;
        unsigned long int start = path.length() - 1;
        
        while (1) {
            if (path[start] != '/') {
                path.pop_back();
                start -= 1;
            } else {
                break;
            }
        }
        
        if (path[start - 1] != '~') {
            path.pop_back();
        }
        
        return pAa.previousPosition;
    }
    
    // Удаляет файл. Принимает точку удаления и размер удаляемого блока
    void deleteFile(int placeToStartDeleting, int offset) {
        // Для начала требуется переписать адреса
        int startPoint = sizeof(int);
        AmountAndPreviousPosition temp;
        Module m;
        
        // Пока не дошли до конца файла
        while (startPoint < getFileSize()) {
            stream.seekg(startPoint, ios::beg);
            stream.read(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
            
            if (temp.previousPosition >= placeToStartDeleting) {
                temp.previousPosition -= offset;
                
                // Записываем изменения в файл
                stream.seekp(startPoint, ios::beg);
                stream.write(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
            }
            
            startPoint += sizeof(AmountAndPreviousPosition) - sizeof(Module);
            for (int i = 0; i < temp.amount; i++) {
                startPoint += sizeof(Module);
                
                // Считали модуль
                stream.seekg(startPoint, ios::beg);
                stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Если это директория и адрес, на который она указывает, стоит дальше, чем точка, в которую мы будем писать
                if (m.type == directory && m.position >= placeToStartDeleting) {
                    // То тогда изменяем этот адрес на размер директории
                    m.position -= offset;
                    
                    // Записываем изменения в файл
                    stream.seekp(startPoint, ios::beg);
                    stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                    
                    continue;
                    
                }
                
                if (m.type == file) {
                    startPoint += m.fileSize;
                }
                
            }
            
            startPoint += sizeof(Module);
            
        }
        
        // Сместили содержимое диска влево на количество удаляемых байт
        shiftLeftOnChar(placeToStartDeleting, offset);
        
        int newSize = getFileSize();
        
        stream.close();
        
        // Обрезаем файл
        FILE *f;
        try {
            if(!(f=fopen(this->diskName, "ab"))) throw 2;
        }
        catch(int) {
            cout << "Не могу открыть файл для обрезки!" << endl;
            return;
        }
        
        ftruncate(fileno(f), newSize - offset);
        fclose(f);
        
        // Открываем файл в бинарном режиме для чтения/записи
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
        
    }
    
    // Удаляет пустую директорию
    void deleteBlockOfOffsetSize(int placeToStartDeleting, int offset) {
        // Для начала требуется переписать адреса
        int startPoint = sizeof(int);
        AmountAndPreviousPosition temp;
        Module m;
        
        // Пока не дошли до конца файла
        while (startPoint < getFileSize()) {
            stream.seekg(startPoint, ios::beg);
            stream.read(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
            
            if (temp.previousPosition >= placeToStartDeleting) {
                temp.previousPosition -= offset;
                
                // Записываем изменения в файл
                stream.seekp(startPoint, ios::beg);
                stream.write(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
            }
            
            startPoint += sizeof(AmountAndPreviousPosition) - sizeof(Module);
            for (int i = 0; i < temp.amount; i++) {
                startPoint += sizeof(Module);
                
                // Считали модуль
                stream.seekg(startPoint, ios::beg);
                stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Если это директория и адрес, на который она указывает, стоит дальше, чем точка, в которую мы будем писать
                if (m.type == directory && m.position >= placeToStartDeleting) {
                    // То тогда изменяем этот адрес на размер директории
                    m.position -= offset;
                    
                    // Записываем изменения в файл
                    stream.seekp(startPoint, ios::beg);
                    stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                    
                    continue;
                    
                }
                
                if (m.type == file) {
                    startPoint += m.fileSize;
                }
                
            }
            
            startPoint += sizeof(Module);
            
        }
        
        // Сместили содержимое диска влево на количество удаляемых байт
        shiftLeftOnChar(placeToStartDeleting, offset);
        
        int newSize = getFileSize();
        
        stream.close();
        
        // Обрезаем файл
        FILE *f;
        try {
            if(!(f=fopen(this->diskName, "ab"))) throw 2;
        }
        catch(int) {
            cout << "Не могу открыть файл для обрезки!" << endl;
            return;
        }
        
        ftruncate(fileno(f), newSize - offset);
        fclose(f);
        
        // Открываем файл в бинарном режиме для чтения/записи
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
    }
    
    // Удаляет модуль с диска
    void deleteModule(int beginning, moduleType whatToDelete, string name, string ext) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        // Если нужно удалить файл
        if (whatToDelete == file) {
            Module m;
            int goThrough = beginning - sizeof(Module);
            // Начинаем поиск этого файла
            for (int i = 0; i < pAa.amount; i++) {
                goThrough += sizeof(Module);
                // Поставили курсор на i-ый модуль
                stream.seekg(goThrough);
                // Считали модуль
                stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Если мы нашли файл
                if (m.type == file) {
                    // И если имя и расширение этого файла совпадают с тем, что нужно удалить
                    if (strcmp(m.name, name.c_str()) == 0 && strcmp(m.extension, ext.c_str()) == 0) {
                        // Получили количество записанной информации
                        int previousSize;
                        
                        stream.seekg(0, ios::beg);
                        stream.read(reinterpret_cast<char*>(&previousSize), sizeof(int));
                        
                        // Изменили её на количество символов, записанных в файл
                        previousSize -= m.fileSize;
                        
                        // Записали эти изменения в файл
                        stream.seekp(0, ios::beg);
                        stream.write(reinterpret_cast<char*>(&previousSize), sizeof(int));
                        
                        // Вызываем функцию, которая удалит файл
                        deleteFile(goThrough, sizeof(Module) + m.fileSize);
                        
                        // Уменьшили счётчик содержимого директории на 1
                        pAa.amount -= 1;
                        
                        // Записали изменения на диск
                        stream.seekp(beginning - sizeof(AmountAndPreviousPosition), ios::beg);
                        stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
                        
                        return;
                    }
                    
                    if (m.type == file) {
                        goThrough += m.fileSize;
                    }
                    
                }
                
            }
            
            cout << "Такого файла нет!" << endl;
            
        // В случае удаления директории
        } else {
            Module m;
            int goThrough = beginning - sizeof(Module);
            // Начинаем поиск этого файла
            for (int i = 0; i < pAa.amount; i++) {
                goThrough += sizeof(Module);
                // Поставили курсор на i-ый модуль
                stream.seekg(goThrough);
                // Считали модуль
                stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Если мы нашли директорию
                if (m.type == directory) {
                    // И если имя директории совпадает с тем, что нужно удалить
                    if (strcmp(m.name, name.c_str()) == 0) {
                        AmountAndPreviousPosition temp;
                        stream.seekg(m.position, ios::beg);
                        stream.read(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
                        
                        // Если директория пустая, то удаляем её
                        if (temp.amount == 0) {
                            // Удаляем дескриптор директории с диска
                            deleteBlockOfOffsetSize(m.position, sizeof(AmountAndPreviousPosition));
                            
                            // Удаляем директорию с диска
                            deleteBlockOfOffsetSize(goThrough, sizeof(Module));
                            
                            // Уменьшили счётчик содержимого директории на 1
                            pAa.amount -= 1;
                            
                            // Записали изменения на диск
                            stream.seekp(beginning - sizeof(AmountAndPreviousPosition), ios::beg);
                            stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
                            
                            return;
                            
                        } else {
                            cout << "Директория не пустая! Удаление невозможно!" << endl;
                            
                            return;
                            
                        }
                        
                    }
                    
                }
                
                if (m.type == file) {
                    goThrough += m.fileSize;
                }
                
            }
            
            cout << "Такой директории нет!" << endl;
            
        }
        
    }
    
    void showAmountOfUsedMemory() {
        // Получили количество записанной информации
        int currentUse;
        
        stream.seekg(0, ios::beg);
        stream.read(reinterpret_cast<char*>(&currentUse), sizeof(int));
        
        cout << "Занято " << currentUse << "/" << getFileSize() << " из " << STORAGE_SIZE << " байт." << endl;
        
    }
    
    void showFileContent(int beginning) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        string show;
        cout << "Введите название файла, который вы хотите открыть:" << endl;
        cin >> show;
        
        string ext;
        cout << "Введите расширение файла, который вы хотите открыть:" << endl;
        cin >> ext;
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == file) {
                if (strcmp(m.name, show.c_str()) == 0 && strcmp(m.extension, ext.c_str()) == 0) {
                    int start = goThrough + sizeof(Module);
                    for (int i = 0; i < m.fileSize; i++) {
                        char c;
                        
                        stream.seekg(start + i * sizeof(char), ios::beg);
                        // Считали информацию
                        stream.read(reinterpret_cast<char*>(&c), sizeof(char));
                        cout << c;
                    }
                    
                    cout << endl;
                    
                    return;
                }
                
                goThrough += m.fileSize;
                
            }
            
        }
        
        cout << "Такого файла нет!" << endl;
    }
    
    // Возвращает размер всех файлов, которые находятся в данной директории
    int getSizeOfAllFilesInDirectory(int beginning) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        int sizeToReturn = 0;
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == file) {
                sizeToReturn += m.fileSize;
                goThrough += m.fileSize;
            }
            
        }
        
        return sizeToReturn;
        
    }
    
    // Удаляет все файлы в заданной директории
    void deleteAllFilesInDirectory(int beginning) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        // Удаляем все файлы из директории
        Module m;
        
        int amount = pAa.amount;
        
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == file) {
                deleteModule(beginning - sizeof(AmountAndPreviousPosition), file, m.name, m.extension);
                amount -= 1;
                
                if (amount > 0 && i + 1 == amount) {
                    i -= 1;
                    goThrough -= sizeof(Module);
                }
                
            }
            
        }
        
        // Теперь начнём удалять содержимое из директорий, которые находятся внутри текущей директории
        goThrough = beginning - sizeof(Module);
        for (int i = 0; i < amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == directory) {
                deleteAllFilesInDirectory(m.position);
                
            }
            
        }
        
        // Теперь начнём удалять сами директории
        goThrough = beginning - sizeof(Module);
        for (int i = 0; i < amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == directory) {
                deleteModule(beginning - sizeof(AmountAndPreviousPosition), directory, m.name, "");
                
                amount -= 1;
                
                if (amount > 0 && i + 1 == amount) {
                    i -= 1;
                    goThrough -= sizeof(Module);
                }
                
            }
            
        }
        
    }
    
    // Переименовывает файлы
    void renameFile(int beginning, string name, string extension) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == file && strcmp(m.name, name.c_str()) == 0 && strcmp(m.extension, extension.c_str()) == 0) {
                Module m1;
                m1 = createFile(renameFileMode);
                
                // Если имя файла уникальное
                if (isUnique(beginning - sizeof(AmountAndPreviousPosition), file, m1) == true) {
                    strcpy(m.name, m1.name);
                    strcpy(m.extension, m1.extension);
                    
                    // Поставили курсор в позицию для записи
                    stream.seekp(goThrough);
                    // Записали обновлённый модуль
                    stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                    
                    return;
                    
                } else {
                    cout << "Такое имя уже существует!" << endl;
                    return;
                }
                
            }
            
            if (m.type == file) {
                goThrough += m.fileSize;
                
            }
            
        }
        
        cout << "Такого файла нет!" << endl;
    }
    
    void renameDirectory(int beginning, string name) {
        
    }
    
    // Удаляем директорию и её содержимое
    void deleteDirectoryWithContent(int beginning, string toDelete) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == directory && strcmp(m.name, toDelete.c_str()) == 0) {
                deleteAllFilesInDirectory(m.position);
                
                deleteModule(beginning - sizeof(AmountAndPreviousPosition), directory, m.name, "");
                
                return;
                
            }
            
            if (m.type == file) {
                goThrough += m.fileSize;
                
            }
            
        }
        
        cout << "Такой директории нет!" << endl;
        
    }
    
    bool isUnique(int beginning, moduleType howToCheck, Module whatToCheck) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Стали на начало модулей
        beginning += sizeof(AmountAndPreviousPosition);
        
        Module m;
        int goThrough = beginning - sizeof(Module);
        // Начинаем поиск этого файла
        for (int i = 0; i < pAa.amount; i++) {
            goThrough += sizeof(Module);
            // Поставили курсор на i-ый модуль
            stream.seekg(goThrough);
            // Считали модуль
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            // Если мы нашли файл
            if (m.type == file) {
                // И если имя и расширение этого файла совпали
                if (strcmp(m.name, whatToCheck.name) == 0 && strcmp(m.extension, whatToCheck.extension) == 0 && howToCheck == file) {
                    return false;
                }
                
                if (m.type == file) {
                    goThrough += m.fileSize;
                }
                
            } else {
                // И если имя и расширение этого файла совпали
                if (strcmp(m.name, whatToCheck.name) == 0 && howToCheck == directory) {
                    return false;
                }
            }
            
        }
        
        return true;
        
    }
    
    // Осуществляет добавление нового элемента на диск
    void writeToDirectory(int beginning, moduleType whatToCreate) {
        int s = getFileSize();
        
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        int check = beginning + sizeof(AmountAndPreviousPosition) + getSizeOfAllFilesInDirectory(beginning) + (pAa.amount * sizeof(Module));
        
        // Если мы стоим в конце наших записей на диске
        if (check == s) {
            // Если нужно создать директорию, то переходим к созданию директории
            if (whatToCreate == directory) {
                if (STORAGE_SIZE - getFileSize() < sizeof(Module) + sizeof(AmountAndPreviousPosition)) {
                    cout << "Не хватает места для создания директории!" << endl;
                    return;
                }
                
                // Для создания директории создаём модуль, описывающий её содержимое
                AmountAndPreviousPosition newDirectoryInfo;
                
                // Количество файлов в новой папке равно 0
                newDirectoryInfo.amount = 0;
                // Адрес предыдущей директории совпадает с началом том директории, в которой мы находимся
                newDirectoryInfo.previousPosition = beginning;
                
                // Создаём директорию
                Module m;
                // В качестве параметра передаём адрес, на который будет указывать директория
                m = createDirectory(beginning + sizeof(AmountAndPreviousPosition) + (pAa.amount + 1) * sizeof(Module) + getSizeOfAllFilesInDirectory(beginning));
                
                if (isUnique(beginning, directory, m) == false) {
                    cout << "Не могу создать директорию, т.к. директория с таким именем существует! Попробуйте ещё раз." << endl;
                    return;
                    
                }
                
                // Записали директорию на диск
                stream.seekp(0, ios::end);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Записали модуль содержимого директории
                stream.seekp(0, ios::end);
                stream.write(reinterpret_cast<char*>(&newDirectoryInfo), sizeof(AmountAndPreviousPosition));
                
                // Увеличиваем счётчик количества модулей на 1
                pAa.amount += 1;
                
                // Записали изменения в файл
                stream.seekg(beginning);
                stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
                
            // В противном случае переходим к созданию файла
            } else {
                if (STORAGE_SIZE - getFileSize() <= sizeof(Module)) {
                    cout << "Не хватает места для создания файла!" << endl;
                    return;
                }
                
                // Создаём файл
                Module m;
                m = createFile(newFileMode);
                
                if (isUnique(beginning, file, m) == false) {
                    cout << "Не могу создать файл, т.к. файл с таким именем существует! Попробуйте ещё раз." << endl;
                    return;
                    
                }
                
                cout << "Введите содержимое файла:" << endl;
                getchar();
                string content;
                getline(cin, content);
                
                if (STORAGE_SIZE - getFileSize() < sizeof(Module) + content.length()) {
                    cout << "Не хватает места для создания файла! Файл будет обрезан." << endl;
                    
                    while (content.length() > STORAGE_SIZE - getFileSize() - sizeof(Module)) {
                        content.pop_back();
                    }
                    
                }
                
                m.fileSize = (int)content.length();
                
                // Записали файл на диск
                stream.seekp(0, ios::end);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Стали в позицию после файла
                stream.seekp(0, ios::end);
                // По-char-oво записываем содержимое файла
                for (int i = 0; i < content.length(); i++) {
                    char c = content[i];
                    stream.seekp(0, ios::end);
                    stream.write(reinterpret_cast<char*>(&c), sizeof(char));
                }
                
                // Увеличиваем счётчик количества модулей на 1
                pAa.amount += 1;
                
                // Записали изменения в файл
                stream.seekg(beginning);
                stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
                
                // Получили количество записанной информации
                int previousSize;
                
                stream.seekg(0, ios::beg);
                stream.read(reinterpret_cast<char*>(&previousSize), sizeof(int));
                
                // Изменили её на количество символов, записанных в файл
                previousSize += content.length();
                
                // Записали эти изменения в файл
                stream.seekp(0, ios::beg);
                stream.write(reinterpret_cast<char*>(&previousSize), sizeof(int));
                
            }
            
        }
        // В случае, если мы стоим в середине наших записей на диске, нам нужно сместить все записи от того места, куда мы будем писать
        else {
            // Проверяем, есть ли место создания директории
            if (STORAGE_SIZE - getFileSize() < sizeof(Module) + sizeof(AmountAndPreviousPosition)) {
                cout << "Не хватает места для создания директории!" << endl;
                return;
            }
            
            // Позиция, в которую мы будем писать
            int whereToWrite = beginning + sizeof(AmountAndPreviousPosition) + pAa.amount * sizeof(Module) + getSizeOfAllFilesInDirectory(beginning);
            
            // Если нужно создать директорию
            if (whatToCreate == directory) {
                // Создаём директорию, которую запишем в файл
                Module newDirectory;
                newDirectory = createDirectory(getFileSize() + sizeof(Module));
                
                if (isUnique(beginning, directory, newDirectory) == false) {
                    cout << "Не могу создать директорию, т.к. директория с таким именем существует! Попробуйте ещё раз." << endl;
                    return;
                    
                }
                
                // Для начала требуется переписать адреса
                int startPoint = sizeof(int);
                AmountAndPreviousPosition temp;
                Module m;
                
                int size = getFileSize();
                if (size < 0) {
                    cout << "Не могу создать директорию!" << endl;
                    return;
                }
                
                // Пока не дошли до конца файла
                while (startPoint < size) {
                    stream.seekg(startPoint, ios::beg);
                    stream.read(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
                    
                    if (temp.previousPosition >= whereToWrite) {
                        temp.previousPosition += sizeof(Module);
                        
                        // Записываем изменения в файл
                        stream.seekp(startPoint, ios::beg);
                        stream.write(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
                    }
                    
                    startPoint += sizeof(AmountAndPreviousPosition) - sizeof(Module);
                    for (int i = 0; i < temp.amount; i++) {
                        startPoint += sizeof(Module);
                        
                        // Считали модуль
                        stream.seekg(startPoint, ios::beg);
                        stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                        
                        // Если это директория и адрес, на который она указывает, стоит дальше, чем точка, в которую мы будем писать
                        if (m.type == directory && m.position >= whereToWrite) {
                            // То тогда изменяем этот адрес на размер директории
                            m.position += sizeof(Module);
                            
                            // Записываем изменения в файл
                            stream.seekp(startPoint, ios::beg);
                            stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                            
                            continue;
                            
                        }
                        
                        if (m.type == file) {
                            startPoint += m.fileSize;
                        }
                        
                    }
                    
                    startPoint += sizeof(Module);
                    
                }
                
                // Сместили все записи правее точки, в которую будем писать
                for (int i = 0; i < sizeof(Module); i++) {
                    shiftRightOnChar(whereToWrite);
                    whereToWrite += 1;
                }
                
                whereToWrite -= sizeof(Module);
                
                // Записали директорию на диск
                stream.seekp(whereToWrite, ios::beg);
                stream.write(reinterpret_cast<char*>(&newDirectory), sizeof(Module));
                
                // Создаём описатель директории
                AmountAndPreviousPosition newDirectoryInfo;
                newDirectoryInfo.amount = 0;
                newDirectoryInfo.previousPosition = beginning;
                
                // Записали описатель директории на диск
                stream.seekp(0, ios::end);
                stream.write(reinterpret_cast<char*>(&newDirectoryInfo), sizeof(AmountAndPreviousPosition));
            
            // В случае, если мы создаём файл
            } else {
                if (STORAGE_SIZE - getFileSize() <= sizeof(Module)) {
                    cout << "Не хватает места для создания файла!" << endl;
                    return;
                }
                
                // Позиция, в которую мы будем писать
                Module newFile;
                newFile = createFile(newFileMode);
                
                if (isUnique(beginning, file, newFile) == false) {
                    cout << "Не могу создать файл, т.к. файл с таким именем существует! Попробуйте ещё раз." << endl;
                    return;
                    
                }
                
                string contentOfTheNewFile;
                cout << "Введите содержимое файла:" << endl;
                getchar();
                string content;
                getline(cin, contentOfTheNewFile);
                
                if (STORAGE_SIZE - getFileSize() < sizeof(Module) + content.length()) {
                    cout << "Не хватает места для создания файла! Файл будет обрезан." << endl;
                    
                    while (content.length() > STORAGE_SIZE - getFileSize() - sizeof(Module)) {
                        content.pop_back();
                    }
                    
                }
                
                newFile.fileSize = (int)contentOfTheNewFile.length();
                
                // На это расстояние нужно переписать содержимое диска от точки записи
                int offset = (int)contentOfTheNewFile.length() + sizeof(Module);
                
                // Для начала требуется переписать адреса
                int startPoint = sizeof(int);
                AmountAndPreviousPosition temp;
                Module m;
                
                int size = getFileSize();
                if (size < 0) {
                    cout << "Не могу создать файл!" << endl;
                    return;
                }
                
                // Пока не дошли до конца файла
                while (startPoint < size) {
                    stream.seekg(startPoint, ios::beg);
                    stream.read(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
                    
                    if (temp.previousPosition >= whereToWrite) {
                        temp.previousPosition += offset;
                        
                        // Записываем изменения в файл
                        stream.seekp(startPoint, ios::beg);
                        stream.write(reinterpret_cast<char*>(&temp), sizeof(AmountAndPreviousPosition));
                    }
                    
                    startPoint += sizeof(AmountAndPreviousPosition) - sizeof(Module);
                    for (int i = 0; i < temp.amount; i++) {
                        startPoint += sizeof(Module);
                        
                        // Считали модуль
                        stream.seekg(startPoint, ios::beg);
                        stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
                        
                        // Если это директория и адрес, на который она указывает, стоит дальше, чем точка, в которую мы будем писать
                        if (m.type == directory && m.position >= whereToWrite) {
                            // То тогда изменяем этот адрес на размер директории
                            m.position += offset;
                            
                            // Записываем изменения в файл
                            stream.seekp(startPoint, ios::beg);
                            stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                            
                            continue;
                            
                        }
                        
                        if (m.type == file) {
                            startPoint += m.fileSize;
                        }
                        
                    }
                    
                    startPoint += sizeof(Module);
                    
                }
                
                // Сместили все записи правее точки, в которую будем писать
                for (int i = 0; i < offset; i++) {
                    shiftRightOnChar(whereToWrite);
                    whereToWrite += 1;
                }
                
                whereToWrite -= offset;
                
                // Записываем описание файла на диск
                stream.seekp(whereToWrite, ios::beg);
                stream.write(reinterpret_cast<char*>(&newFile), sizeof(Module));
                
                whereToWrite += sizeof(Module);
                
                // Записываем содержимое файла на диск
                for (int i = 0; i < contentOfTheNewFile.length(); i++) {
                    char c = contentOfTheNewFile[i];
                    
                    stream.seekp(whereToWrite, ios::beg);
                    stream.write(reinterpret_cast<char*>(&c), sizeof(char));
                    
                    whereToWrite += 1;
                    
                }
                
                // Получили количество записанной информации
                int previousSize;
                
                stream.seekg(0, ios::beg);
                stream.read(reinterpret_cast<char*>(&previousSize), sizeof(int));
                
                // Изменили её на количество символов, записанных в файл
                previousSize += contentOfTheNewFile.length();
                
                // Записали эти изменения в файл
                stream.seekp(0, ios::beg);
                stream.write(reinterpret_cast<char*>(&previousSize), sizeof(int));
                
            }
            
            // Увеличиваем счётчик количества модулей на 1
            pAa.amount += 1;
            
            // Записали изменения в файл
            stream.seekg(beginning);
            stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
            
        }
        
    }
};

#endif /* Writer_hpp */
