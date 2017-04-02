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
#include <unistd.h> 
#include <stdio.h>

#include <iostream>
using namespace std;

//Флаг ожидания клиента
bool usr_interrupt = 0;
//Обработчик сигналов
void synch_signal (int sig) {
  usr_interrupt = 1;
}

MySpace s;
int main(int argc, char *argv[]) {
    fstream file;
    //Считываем PID-ы клиентов из файла
    file.open("ClientPid.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    file.close();

    //Записываем свой PID в файл
    file.open("ServerPid.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    int temp = getpid();
    file.seekp(0, ios_base::beg);
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));
    file.close();

    //Записали точку отсчёта для имён файлов
    file.open("Number.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    temp = 0;
    file.seekp(0, ios_base::beg);
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));
    file.close();

    struct sigaction usr_action;
    sigset_t block_mask;
    pid_t process;

    sigfillset (&block_mask);
    usr_action.sa_handler = synch_signal;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    //Количество клиентов
    int amountOfUsers = 0;

    cout << "Нажмите Enter, когда клиенты войдут." << endl;
    getchar(); 

    //Считываем PID-ы клиентов из файла
    file.open("ClientPid.txt", ios::in | ios::out | ios::binary);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }

    //Получили количество клиентов
	file.seekg(0, ios_base::end);
    amountOfUsers = file.tellg() / sizeof(int);
    
    //Массив для хранения PID-ов клиентов
    int *pid = new int[amountOfUsers];

    for (int i = 0; i < amountOfUsers; i++) {
    	file.seekg(i * sizeof(int), ios_base::beg);
	    file.read(reinterpret_cast<char*>(&(pid[i])), sizeof(int));
	}
    file.close();

    //Создаём файлы сохранения, если они ещё не созданы
    cout << "Вы хотите создать файлы с продуктами (y/n)?" << endl;
    char c;
    cin >> c;
    if (c == 'y') {
        s.createFiles();
    }

    for (int i = 0; i < amountOfUsers; i++) {
        cout << endl << "Пользователь с PID " << pid[i] << endl;
    	//Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(pid[i], SIGUSR1);
    	//Ожидаем, пока клиент не выполнит свои действия
        while (!usr_interrupt);
        usr_interrupt = 0;

    	//Выводим сообщение, что сервер ожидает ввода денег пользователя
        cout << "Ожидаю ввода денег..." << endl;

        //Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(pid[i], SIGUSR1);
        //Ожидаем, пока клиент не выполнит свои действия
        while (!usr_interrupt);
        usr_interrupt = 0;
        
        //Создаём уникальное имя для файла с деньгами
        char moneyName[2];
        moneyName[0] = (i + 1) + '0';

        //Выводим количество денег, которое было введено в машину и показываем наш товар 
        s.showAmountOfMoneyInVendingMachine(moneyName);
        s.showProducts();

        //Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(pid[i], SIGUSR1);
        //Ожидаем, пока клиент не выполнит свои действия
        while (!usr_interrupt);
        usr_interrupt = 0;

        //Выполняем заказ клиента
        s.performOrder(moneyName);

        //Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(pid[i], SIGUSR1);
  	}
    
    return 0;
}
