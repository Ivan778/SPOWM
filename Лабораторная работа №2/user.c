#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <fstream>
#include "MyLibrary/CreateFiles.hpp"
#include "MyLibrary/Product.hpp"

#include <iostream>
using namespace std;

bool usr_interrupt = 0;

void synch_signal (int sig) {
  usr_interrupt = 1;
}

MySpace s;
int main(int argc, char *argv[]) {
 	fstream file;
    //Записываем свой PID клиента в файл
    file.open("ClientPid.txt", ios::in | ios::out | ios::binary);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    int temp = getpid();
    //Записали pid
    file.seekp(0, ios_base::end);
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));

    file.close();

    //Создаём уникальное имя файла с деньгами
    char moneyName[2];
    file.open("Number.txt", ios::in | ios::out | ios::binary);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    temp = 0;
    file.seekg(0, ios_base::beg);
    file.read(reinterpret_cast<char*>(&temp), sizeof(int));

    moneyName[0] = (temp + 1) + '0';

    temp = 1;

    file.seekp(0, ios_base::beg);
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));

    file.close();

    //Получаем PID сервера
    file.open("ServerPid.txt", ios::in | ios::out | ios::binary);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    int server;
    file.seekg(0, ios_base::beg);
    file.read(reinterpret_cast<char*>(&server), sizeof(int));
    file.close();

    struct sigaction usr_action;
    sigset_t block_mask;

    sigfillset (&block_mask);
    usr_action.sa_handler = synch_signal;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    cout << "Ожидаю ответ от сервера..." << endl;

    //Ожидаем, пока сервер не выполнит свои действия
    while (!usr_interrupt) {
        fflush(stdin);
    }
    usr_interrupt = 0;

    cout << "Вы хотите создать файл с деньгами (y/n)?" << endl;
    char c;
    cin >> c;
    if (c == 'y') {
        s.createMoney(moneyName);
    }

    //Отправили на сервер сигнал, который позволяет ему продолжить работу
    kill(server, SIGUSR1);
    //Ожидаем, пока сервер не выполнит свои действия
    while (!usr_interrupt);
    usr_interrupt = 0;

    //Просим пользователя ввести деньги
    s.askManToPutMoney(moneyName);

    //Отправили на сервер сигнал, который позволяет ему продолжить работу
    kill(server, SIGUSR1);
    //Ожидаем, пока сервер не выполнит свои действия
    while (!usr_interrupt);
    usr_interrupt = 0;

    //Просим пользователя ввести продукты
    s.askManToEnterProducts();

    //Отправили на сервер сигнал, который позволяет ему продолжить работу
    kill(server, SIGUSR1);
    //Ожидаем, пока сервер не выполнит свои действия
    while (!usr_interrupt);
    usr_interrupt = 0;

    s.showPurchases(moneyName);


    return 0;
}
