//
//  main.cpp
//  SPO#4
//
//  Created by Иван on 12.04.17.
//  Copyright © 2017 IvanCode. All rights reserved.
//

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

bool stopAllThreads = false;
bool stopPrint = false;

int mainCounter = 0;
int tempCounter = 0;
int threadToDelete = -1;

int createDelay = 1000;
int showDelay = 1;

bool printEndl = false;

void* threadFunc(void *args) {
    int myNumber = mainCounter++;
    
    //Пока не было команды завершить все потоки
    while (stopAllThreads == false) {
        if ((printEndl == true) && (stopPrint == false)) {
            sleep(showDelay);
        }
        
        //Если печать разрешена
        if ((stopPrint == false) && (tempCounter == myNumber)) {
            cout << myNumber << endl;
            sleep(1);
            tempCounter++;
            
        }
        
        if ((stopPrint == false) && (tempCounter >= mainCounter) && (tempCounter == myNumber + 1)) {
            cout << endl;
            
            tempCounter = 1000;
            
            sleep(showDelay);
            
            tempCounter = 0;
        }
        
        if (myNumber == threadToDelete) {
            mainCounter = myNumber;
            threadToDelete = -1;
            break;
        }
        
    }
    
    pthread_exit(0);
}

void* createThreads(void *args) {
    //Создание потока
    //Какие-то данные для потока
    void* threadData = NULL;
    
    //Создаем идентификатор потока
    pthread_t thread;
    
    while(stopAllThreads == false) {
        //Создаем поток по идентификатору thread и функции потока threadFunc и передаем потоку указатель на данные thread_data
        pthread_create(&thread, NULL, threadFunc, threadData);
        
        //Засыпаем перед созданием следующего потока
        sleep(createDelay);
    }
    
    pthread_exit(0);
}

int main(int argc, const char * argv[]) {
    //Создание потока
    //Какие-то данные для потока
    void* threadData = NULL;
    
    //Создаем идентификатор потока
    pthread_t thread;
    pthread_t addThread;
    
    //Создание потока, который будет создавать другие потоки
    pthread_create(&thread, NULL, createThreads, threadData);
    
    char c;
    while (stopAllThreads == false) {
        cin >> c;
        
        switch (c) {
            case 's': {
                stopAllThreads = true;
                break;
            }
            case 'd': {
                stopPrint = true;
                
                threadToDelete = mainCounter - 1;
                mainCounter -= 1;
                //tempCounter = 0;
                
                stopPrint = false;
                
                break;
            }
            case 'a': {
                stopPrint = true;
                
                //tempCounter = 0;
                pthread_create(&thread, NULL, threadFunc, threadData);
                stopPrint = false;
                
                break;
            }
            default: {
                break;
            }
        }
    }
    
    pthread_join(thread, NULL);
    
    return 0;
}
