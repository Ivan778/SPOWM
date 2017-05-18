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
        stream.seekg(0, ios::end);
        return (int)stream.tellg();
    }
    
    Module createDirectory(int position) {
        Module m;
        
        cout << "Введите название новой директории: " << endl;
        cin >> m.name;
        
        m.type = directory;
        m.position = position;
        
        return m;
    }
    
    Module createFile() {
        Module m;
        
        cout << "Введите название нового файла (до 20 символов): " << endl;
        cin >> m.name;
        
        cout << "Введите расширение (до 5 символов): " << endl;
        cin >> m.extension;
        
        m.type = file;
        
        return m;
    }
    
    void showModule(Module m) {
        if (m.type == file) {
            cout << m.name << "." << m.extension << " - файл" << endl;
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
            
            goThrough += m.fileSize;
            
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
        for (int i = beginning + 52; i < getFileSize(); i++) {
            char c;
            stream.seekg(i, ios::beg);
            stream.read(reinterpret_cast<char*>(&c), sizeof(char));
            
            stream.seekp(i - 52, ios::beg);
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
    
    // Удаляет модуль с диска
    void deleteModule(int beginning, moduleType whatToDelete) {
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
            string show;
            cout << "Введите название файла, который вы хотите удалить:" << endl;
            cin >> show;
            
            string ext;
            cout << "Введите расширение файла, который вы хотите удалить:" << endl;
            cin >> ext;
            
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
                    if (strcmp(m.name, show.c_str()) == 0 && strcmp(m.extension, ext.c_str()) == 0) {
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
                    
                    goThrough += m.fileSize;
                    
                }
                
            }
            
            cout << "Такого файла нет!" << endl;
            
        // В случае удаления директории
        } else {
            
        }
        
    }
    
    void showAmountOfUsedMemory() {
        // Получили количество записанной информации
        int currentUse;
        
        stream.seekg(0, ios::beg);
        stream.read(reinterpret_cast<char*>(&currentUse), sizeof(int));
        
        cout << "Занято " << currentUse << " из " << STORAGE_SIZE << " байт." << endl;
        
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
            
            goThrough += m.fileSize;
            
            if (m.type == file) {
                sizeToReturn += m.fileSize;
            }
            
        }
        
        return sizeToReturn;
        
    }
    
    // Осуществляет добавление нового элемента на диск
    void writeToDirectory(int beginning, moduleType whatToCreate) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        // Если мы стоим в конце наших записей на диске
        if (beginning + sizeof(AmountAndPreviousPosition) + getSizeOfAllFilesInDirectory(beginning) + (pAa.amount * sizeof(Module)) == getFileSize()) {
            // Если нужно создать директорию, то переходим к созданию директории
            if (whatToCreate == directory) {
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
                // Увеличиваем счётчик количества модулей на 1
                pAa.amount += 1;
                
                // Записали изменения в файл
                stream.seekg(beginning);
                stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
                
                // Создаём файл
                Module m;
                m = createFile();
                
                cout << "Введите содержимое файла:" << endl;
                getchar();
                string content;
                getline(cin, content);
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
            // Позиция, в которую мы будем писать
            int whereToWrite = beginning + sizeof(AmountAndPreviousPosition) + pAa.amount * sizeof(Module) + getSizeOfAllFilesInDirectory(beginning);
            
            // Если нужно создать директорию
            if (whatToCreate == directory) {
                // Для начала требуется переписать адреса
                int startPoint = sizeof(int);
                AmountAndPreviousPosition temp;
                Module m;
                
                // Пока не дошли до конца файла
                while (startPoint < getFileSize()) {
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
                        
                        startPoint += m.fileSize;
                        
                    }
                    
                    startPoint += sizeof(Module);
                    
                }
                
                // Сместили все записи правее точки, в которую будем писать
                for (int i = 0; i < sizeof(Module); i++) {
                    shiftRightOnChar(whereToWrite);
                    whereToWrite += 1;
                }
                
                whereToWrite -= sizeof(Module);
                
                // Создаём директорию. которую запишем в файл
                Module newDirectory;
                newDirectory = createDirectory(getFileSize());
                
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
                // Позиция, в которую мы будем писать
                Module newFile;
                newFile = createFile();
                
                string contentOfTheNewFile;
                cout << "Введите содержимое файла:" << endl;
                getchar();
                string content;
                getline(cin, contentOfTheNewFile);
                newFile.fileSize = (int)contentOfTheNewFile.length();
                
                // На это расстояние нужно переписать содержимое диска от точки записи
                int offset = (int)contentOfTheNewFile.length() + sizeof(Module);
                
                // Для начала требуется переписать адреса
                int startPoint = sizeof(int);
                AmountAndPreviousPosition temp;
                Module m;
                
                // Пока не дошли до конца файла
                while (startPoint < getFileSize()) {
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
                        
                        startPoint += m.fileSize;
                        
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
