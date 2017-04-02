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
#include <deque>

#include <iostream>
using namespace std;

//Флаг ожидания клиента
bool usr_interrupt = 0;
//Обработчик сигналов
void synch_signal (int sig) {
  usr_interrupt = 1;
}

bool f = true;
int start = 0;

//Функция потока для вывода PID-ов подключённых пользователей
void* threadFunc(void* thread_data) {
	//Штучка для использования файлов
    fstream file1;
	while(f) {
		file1.open("ClientPid1.txt", ios::in | ios::out | ios::binary);
	    if(!file1.is_open()) {
	        cout << "Файл (ClientPid1) не открыт!" << endl;
	        return 0;
	    }
	    file1.seekg(0, ios::end);
	    int size = file1.tellg() / sizeof(int);

	  	for (int j = start; j < size; j++) {
	  		int temp1;
	  		file1.seekg(j * sizeof(int), ios_base::beg);
    		file1.read(reinterpret_cast<char*>(&temp1), sizeof(int));
	  		cout << temp1 << endl;
	  	}
	  	cout << endl;

	    file1.close();
	    sleep(1);
	}
	pthread_exit(0);
}

MySpace s;
int main(int argc, char *argv[]) {
	//Очередь для хранения PID-ов

    //Создание потока
    //Какие то данные для потока (для примера)
	void* thread_data = NULL;
 
	//Создаем идентификатор потока
	pthread_t thread;
	
	//Штучка для использования файлов
    fstream file;
    //Файл для хранения PID-ов клиентов (для основного потока)
    file.open("ClientPid.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ClientPid) не открыт!" << endl;
        return 0;
    }
    file.close();

    //Файл для хранения PID-ов клиентов (для потока, который отслеживает подключённых пользователей)
    file.open("ClientPid1.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ClientPid1) не открыт!" << endl;
        return 0;
    }
    file.close();

    //Записываем свой PID в файл
    file.open("ServerPid.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (ServerPid) не открыт!" << endl;
        return 0;
    }
    int temp = getpid();
    file.seekp(0, ios_base::beg);
    file.write(reinterpret_cast<char*>(&temp), sizeof(int));
    file.close();

    //Записали точку отсчёта для имён файлов
    file.open("Number.txt", ios::in | ios::out | ios::binary | ios::trunc);
    if(!file.is_open()) {
        cout << "Файл (NumberPid) не открыт!" << endl;
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

    //cout << "Нажмите Enter, когда надо начать работу." << endl;
    //getchar(); 

    //Создаём файлы сохранения, если они ещё не созданы
    cout << "Вы хотите создать файлы с продуктами (y/n)?" << endl;
    char c;
    cin >> c;
    if (c == 'y') {
        s.createFiles();
    }

    //Создаем поток по идентификатору thread и функции потока threadFunc и передаем потоку указатель на данные thread_data
	pthread_create(&thread, NULL, threadFunc, thread_data);

    int i = 0;
    while (1) {
    	//Получаем PID очередного клиента
    	file.open("ClientPid.txt", ios::in | ios::out | ios::binary);
	    if(!file.is_open()) {
	        cout << "Файл (ClientPid) не открыт!" << endl;
	        return 0;
	    }
	    int temp;
		file.seekg(0, ios_base::end);
		temp = file.tellg() / sizeof(int);
        if (i == temp) {
            cout << "Пользователей нет! Продолжить работу? (y/n)" << endl;
            char c;
            cin >> c;
            if (c == 'y') {
                file.close();
                continue;
            }
            else {
                file.close();
                break;
            }
        }
 
	    file.seekg(i * sizeof(int), ios_base::beg);
    	file.read(reinterpret_cast<char*>(&temp), sizeof(int));
    	file.close();

        cout << endl << "Пользователь с PID " << temp << endl;
    	//Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(temp, SIGUSR1);
    	//Ожидаем, пока клиент не выполнит свои действия
        while (!usr_interrupt);
        usr_interrupt = 0;

    	//Выводим сообщение, что сервер ожидает ввода денег пользователя
        cout << "Ожидаю ввода денег..." << endl;

        //Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(temp, SIGUSR1);
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
        kill(temp, SIGUSR1);
        //Ожидаем, пока клиент не выполнит свои действия
        while (!usr_interrupt);
        usr_interrupt = 0;

        //Выполняем заказ клиента
        s.performOrder(moneyName);

        //Отправили на клиент сигнал, который позволяет ему продолжить работу
        kill(temp, SIGUSR1);

        //Увеличили индекс прохода по файлу
        start = ++i;
  	}

  	//Говорим потоку, что пришла пора закончить работу
  	f = false;

  	//Ждём, когда поток завершит работу
  	pthread_join(thread, NULL);
    
    return 0;
}
