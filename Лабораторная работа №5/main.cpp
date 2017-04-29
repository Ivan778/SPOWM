//
//  main.cpp
//  SPO5
//
//  Created by Иван on 22.04.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#include "MySpace.hpp"

int main(int argc, const char * argv[]) {
    //Вектор для хранения имён файлов
    vector<string> files;
    //Получили список файлов
    MySpace::getContentOfTheDirectory("FilesToRead", files);
    //Удалили из него первые два элемента
    files.erase(files.begin());
    files.erase(files.begin());
    
    MySpace::BufferAndFlags bf;
    bf.isReaderReady = false;
    bf.isAllFilesProcessed = false;
    bf.isWriterReady = true;
    bf.fileNames = files;
    bf.outputFileName = "Output/Output.txt";
    
    //Создаем идентификатор потока
    pthread_t reader;
    
    //
    pthread_t writer;
    
    //Создание потока, который будет создавать другие потоки
    pthread_create(&reader, NULL, MySpace::reader, &bf);
    
    //
    pthread_create(&writer, NULL, MySpace::writer, &bf);
    
    //Ожидаем завершение потока
    pthread_join(reader, NULL);
    //Ожидаем завершение потока
    pthread_join(writer, NULL);
    
    return 0;
}
