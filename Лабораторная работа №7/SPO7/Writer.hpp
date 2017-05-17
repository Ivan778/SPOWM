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
using namespace std;

// Вместимость диска
const int LENGTH = 100000;

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
    
    // Форматирует диск (просто записываем в начало диска структуру типа АmountAndPreviousPosition, которая говорит, что в корне ничего не записано, т.е. количество файлов и папок равно нулю)
    void diskFormatting() {
        AmountAndPreviousPosition firstWrite;
        firstWrite.amount = 0;
        firstWrite.previousPosition = 0;
        
        stream.close();
        // Открываем файл в бинарном режиме для чтения/записи и чистим его
        stream.open(this->diskName, ios_base::in | ios_base::out | ios_base::binary | ios::trunc);
        
        // Проверка на открытие
        if (!stream.is_open()) {
            cout << "Не могу открыть файл!" << endl;
            return;
        }
        
        stream.seekp(0, ios::beg);
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
        for (int i = 0; i < pAa.amount; i++) {
            // Поставили курсор на i-ый модуль
            stream.seekg(beginning + i * sizeof(Module));
            //
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            showModule(m);
        }
    }
    
    // Переходим в другую директорию по имени
    int changeDirectory(int beginning) {
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
        for (int i = 0; i < pAa.amount; i++) {
            // Поставили курсор на i-ый модуль
            stream.seekg(beginning + i * sizeof(Module));
            //
            stream.read(reinterpret_cast<char*>(&m), sizeof(Module));
            
            if (m.type == directory) {
                if (strcmp(m.name, goTo.c_str()) == 0) {
                    // Вернули позицию директории, в которую нужно перейти
                    return m.position;
                }
            }
        }
        
        cout << "Такой папки в текущей директории нет!" << endl;
        return -1;
        
    }
    
    // Осуществялет сдвиг всех данных в файле вправо на размер один char начиная с указанной точки
    void shiftRightOnChar(int beginning) {
        for (int i = getFileSize(); i > beginning; i--) {
            char c;
            stream.seekg(i - 1, ios::beg);
            stream.read(reinterpret_cast<char*>(&c), sizeof(char));
            
            stream.seekp(i, ios::beg);
            stream.write(reinterpret_cast<char*>(&c), sizeof(char));
        }
    }
    
    // Позволяет осуществлять переход в предыдущую директорию
    int goToPreviousDirectory(int beginning) {
        // Здесь будем хранить информацию о количестве элементов и предыдущей папке
        AmountAndPreviousPosition pAa;
        // Стали на нужную позицию в файле
        stream.seekg(beginning, ios::beg);
        // Считали информацию
        stream.read(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
        
        return pAa.previousPosition;
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
        if (beginning + sizeof(AmountAndPreviousPosition) + (pAa.amount * sizeof(Module)) == getFileSize()) {
            // Увеличиваем счётчик количества модулей на 1
            pAa.amount += 1;
            
            // Записали изменения в файл
            stream.seekg(beginning);
            stream.write(reinterpret_cast<char*>(&pAa), sizeof(AmountAndPreviousPosition));
            
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
                m = createDirectory(beginning + sizeof(AmountAndPreviousPosition) + pAa.amount * sizeof(Module));
                
                // Записали директорию на диск
                stream.seekp(beginning + sizeof(AmountAndPreviousPosition) + (pAa.amount - 1) * sizeof(Module), ios::beg);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Записали модуль содержимого директории
                stream.seekp(beginning + sizeof(AmountAndPreviousPosition) + pAa.amount * sizeof(Module), ios::beg);
                stream.write(reinterpret_cast<char*>(&newDirectoryInfo), sizeof(AmountAndPreviousPosition));
                
            // В противном переходим к созданию файла
            } else {
                // Создаём файл
                Module m;
                m = createFile();
                
                // Записали файл на диск
                stream.seekp(beginning + sizeof(AmountAndPreviousPosition) + (pAa.amount - 1) * sizeof(Module), ios::beg);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
            }
            
        }
        // В случае, если мы стоим в середине наших записей на диске, нам нужно сместить все записи от того места, куда мы будем писать
        else {
            // Позиция, в которую мы будем писать
            int whereToWrite = beginning + sizeof(AmountAndPreviousPosition) + pAa.amount * sizeof(Module);
            
            // С помощью него будем ходить по диску
            int tempCounter = beginning;
            
            Module tempModule;
            AmountAndPreviousPosition tempPAa;
            
            while(tempCounter != getFileSize()) {
                // Получаем информацию об i-ой директории
                stream.seekg(tempCounter);
                stream.read(reinterpret_cast<char*>(&tempPAa), sizeof(AmountAndPreviousPosition));
                
                //
                if (tempPAa.previousPosition >= whereToWrite) {
                    tempPAa.previousPosition += sizeof(Module);
                    
                    // Записали изменения в файл
                    stream.seekp(tempCounter);
                    stream.write(reinterpret_cast<char*>(&tempPAa), sizeof(AmountAndPreviousPosition));
                }
                
                tempCounter += sizeof(AmountAndPreviousPosition);
                
                for (int i = 0; i < tempPAa.amount; i++) {
                    stream.seekg(tempCounter + i * sizeof(Module));
                    stream.read(reinterpret_cast<char*>(&tempModule), sizeof(Module));
                    
                    // Если мы считали директорию
                    if (tempModule.type == directory) {
                        // Если эта директория ссылается на директорию, которая записана после места, куда нужно писать
                        if (tempModule.position >= whereToWrite) {
                            // Изменяем ссылку на данные
                            tempModule.position += sizeof(Module);
                            
                            // Записали изменения в файл
                            stream.seekp(tempCounter + i * sizeof(Module));
                            stream.write(reinterpret_cast<char*>(&tempModule), sizeof(Module));
                            
                        }
                    }
                    
                }
                
                // Изменили значение счётчика
                tempCounter += tempPAa.amount * sizeof(Module);
                
            }
            
            int shiftTemp = whereToWrite;
            
            // Сдвигаем все данные на размер одного модуля
            for (int i = 0; i < 36; i++) {
                shiftRightOnChar(shiftTemp);
                shiftTemp += 1;
            }
            
            // Если мы создаём файл
            if (whatToCreate == file) {
                // Создаём файл
                Module m;
                m = createFile();
                
                // Записали файл на диск
                stream.seekp(whereToWrite);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
            // В случае создания директории
            } else {
                // Для создания директории создаём модуль, описывающий её содержимое
                AmountAndPreviousPosition newDirectoryInfo;
                
                // Количество файлов в новой папке равно 0
                newDirectoryInfo.amount = 0;
                // Адрес предыдущей директории совпадает с началом том директории, в которой мы находимся
                newDirectoryInfo.previousPosition = beginning;
                
                // Создаём директорию
                Module m;
                // В качестве параметра передаём адрес, на который будет указывать директория
                m = createDirectory(getFileSize());
                
                // Записали директорию на диск
                stream.seekp(whereToWrite);
                stream.write(reinterpret_cast<char*>(&m), sizeof(Module));
                
                // Записали модуль содержимого директории
                stream.seekp(getFileSize());
                stream.write(reinterpret_cast<char*>(&newDirectoryInfo), sizeof(AmountAndPreviousPosition));
                
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
