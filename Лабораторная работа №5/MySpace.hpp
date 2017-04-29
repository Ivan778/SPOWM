//
//  MySpace.cpp
//  SPO5
//
//  Created by Иван on 29.04.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#include <sys/stat.h>
#include <sys/types.h>
#include <aio.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <mutex>
#include <dirent.h>
#include <vector>

using namespace std;

namespace MySpace {
    struct BufferAndFlags {
        //Передаваемый буфер со считанными из файла данными
        string buffer;
        //Флаг, который сигнализирует о готовности читателя (т.е. писатель может начать писать)
        bool isReaderReady;
        //Флаг, который сигнализирует о том, что все файлы были вычитаны
        bool isAllFilesProcessed;
        
        //Вектор, который будет хранить список файлов в каталоге
        vector<string> fileNames;
        //Флаг, который сигнализирует о готовности писателя
        bool isWriterReady;
        
        string outputFileName;
        
    };
    
    const int amountOfBytesToRead = 10;
    
    //Поток будет читать из файла
    void* reader(void* cb) {
        
        cout << "Начинаю чтение" << endl;
        
        //Структура для передачи данных
        BufferAndFlags *f = (BufferAndFlags*)cb;
        //Буфер, куда информация будет вычитываться
        char *buffer = new char[amountOfBytesToRead];
        //Структура для асинхронного чтения из файла
        aiocb b;
        
        int fileNumber = 0;
        while (fileNumber < f->fileNames.size()) {
            //Создаём имя файла
            string name = "FilesToRead/" + f->fileNames[fileNumber];
            
            //Файловый дескриптор
            int fileDescriptor = open(name.c_str(), O_RDONLY, 0);
            if (fileDescriptor == -1) {
                f->isReaderReady = true;
                cout << "Не могу открыть файл" << endl;
                pthread_exit(0);
            }
            
            //Место, откуда будем вычитывать файл
            int offset = 0;
            
            while(1) {
                //Инициализируем структуру aiocb
                memset(&b, 0, sizeof(aiocb));
                b.aio_nbytes = amountOfBytesToRead;
                b.aio_fildes = fileDescriptor;
                b.aio_offset = offset;
                b.aio_buf = buffer;
                
                //Запускаем процесс чтения из файла
                if (aio_read(&b) == -1) {
                    cout << "Проблемы с чтением из файла" << endl;
                    close(fileDescriptor);
                }
                
                //Ожидаем, пока файл не вычитают
                while(aio_error(&b) == EINPROGRESS);
                //Получаем количество считанных символов (байтов)
                int numBytes = (int)aio_return(&b);
                //Записываем их в буфер
                for (int i = 0; i < numBytes; i++) {
                    f->buffer += buffer[i];
                }
                
                //Если количество считанных символов меньше, чем размер буфера для считывания, то выходим из цикла
                if (numBytes < amountOfBytesToRead) {
                    break;
                }
                
                //Увеличиваем отступ от начала файла
                offset += amountOfBytesToRead;
            }
            
            //Говорим писателю, что тот может начать писать
            f->isReaderReady = true;
            //Говорим, что писатель занят
            f->isWriterReady = false;
            
            //Выбираем следующий файл из списка
            fileNumber++;
            
            //Закрываем файл предыдущий
            close(fileDescriptor);
            
            //Ждём, пока отработает писатель
            while (f->isWriterReady == false);
            
        }
        
        f->isAllFilesProcessed = true;
        
        pthread_exit(0);
    }
    
    //Поток будет писать в файла
    void* writer(void* cb) {
        //Структура для передачи данных
        BufferAndFlags *f = (BufferAndFlags*)cb;
        
        //Структура, с помощью которой узнаем размер файла
        struct stat st;
        stat(f->outputFileName.c_str(), &st);
        
        //Место, куда будем писать. Узнали размер файла => в эту точку и будем писать, т.к. нужно совершить конкатенацию
        int offset = (int)st.st_size;
        
        //Файловый дескриптор
        int fileDescriptor = open(f->outputFileName.c_str(), O_RDWR);
        if (fileDescriptor == -1) {
            pthread_exit(0);
            cout << "Не могу открыть файл" << endl;
            pthread_exit(0);
        }
        
        //Структура для асинхронной записи в файл
        aiocb b;
        
        //Пока не прочитали все файлы
        while (f->isAllFilesProcessed == false) {
            //Ждём читателя
            while (f->isReaderReady == false && f->isAllFilesProcessed == false);
            
            //Выводим содержимое, считанное из файла
            cout << offset << ":" << f->buffer << endl;
            
            //Переписываем в char* содержимое, которое нужно записать в файл
            char *buffer = new char[f->buffer.size() + 1];
            strcpy(buffer, f->buffer.c_str());
            
            //Инициализируем структуру aiocb
            memset(&b, 0, sizeof(aiocb));
            b.aio_fildes = fileDescriptor;
            b.aio_nbytes = f->buffer.size() + 1;
            b.aio_offset = offset;
            b.aio_buf = (void*)buffer;
            b.aio_sigevent.sigev_notify = SIGEV_NONE;
            
            //Запускаем процесс записи в файл
            if (aio_write(&b) == -1) {
                cout << "Проблемы с записью в файл!" << endl;
                close(fileDescriptor);
                
                //Чистим буфер
                f->buffer = "";
                
                //Говорим, что читатель может приступить к работе
                f->isReaderReady = false;
                //Писатель будет опять ждать
                f->isWriterReady = true;
                
                //Выводим имя ошибки
                cout << aio_error(&b) << endl;
                
                continue;
            }
            
            //Ожидаем, пока запись не окончится
            while(aio_error(&b) == EINPROGRESS);
            
            cout << "Закончил запись!" << endl;
            
            //Изменяем позицию для записи в файл
            offset += f->buffer.size();
            //Чистим буфер
            f->buffer = "";
            
            //Говорим, что читатель снова занят
            f->isReaderReady = false;
            //Говорим, что писатель ждёт => читатель начинает свою работу
            f->isWriterReady = true;
            
            //Это просто мегаобязательно!!! Возвращает статус асинхронной операции записи (-1 - ошибка, если операция не завершена - неизвестно что)
            //Если её включить в код, то последующая запись в файл будет недоступна
            aio_return(&b);
            
            //Удаляем буфер
            delete [] buffer;
            
        }
        
        close(fileDescriptor);
        
        pthread_exit(0);
    }
    
    //Вычитывает имена файлов из папки
    int getContentOfTheDirectory(string dir, vector<string> &files) {
        DIR *dp;
        struct dirent *dirp;
        if((dp  = opendir(dir.c_str())) == NULL) {
            cout << "Error(" << errno << ") opening " << dir << endl;
            return errno;
        }
        
        while ((dirp = readdir(dp)) != NULL) {
            files.push_back(string(dirp->d_name));
        }
        closedir(dp);
        return 0;
    }
    
    
    
}
