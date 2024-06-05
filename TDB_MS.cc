#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

using namespace std;

#define REG_CHAN "registration_chanel"
bool DEBUG = true;
bool ShutDown = false;
volatile sig_atomic_t stop_thread = 0;

int tik_count = 0;//	Счетчик тиков

int mainPid = getpid();

//************Структура для запроса на регистрацию********
typedef struct _pulse msg_header_t; // абстрактный тип для заголовка сообщения как у импульса
typedef struct _my_data {
	msg_header_t hdr;
	string name; // имя СУБТД
	int pid; // 	id процесса
	int tid; // 	id таймера
} reg_msg_t;

static void* client(void*);//						Заглушка. Имитирует СУБТД посылающую запрос на регистрацию
static void handler(int sig);//						Обработчик сигнала SignalKill


int main() {
	cout << "MainPid: " << mainPid << endl;

	if ((pthread_create(NULL, NULL, &client, NULL)) != EOK) {
		perror("Main: 	thread create ERROR\n");
		exit(EXIT_FAILURE);
	};

	if (DEBUG) {
		cout << "Main dbg: 	client thread starting complete" << endl;
	}

	while (!ShutDown){

	}
}

//-------------------------------------------------------
//*****************Заглушка******************************
//-------------------------------------------------------
static void* client(void*) {
	if (DEBUG) {
		printf("Client debug: Client is running...\n");
	}

	// Установка обработчика сигнала
	signal(SIGUSR1, handler);

	reg_msg_t msg;

	/* Заголовок сообщения клиента */
	msg.hdr.type = 1;
	msg.hdr.subtype = 1;

	cout << endl << "Client	  : ----tdb created---" << endl;
	msg.name = "TemporalDB_MS_1";
	cout << "Client   : 	Name		: " << msg.name << endl;
	msg.pid = getpid();
	cout << "Client	  : 	Process ID 	: " << msg.pid << endl;
	msg.tid = gettid();
	cout << "Client	  : 	Thread ID 	:  " << msg.tid << endl << endl;
	;
	cout << "Client	  : ----tdb created---" << endl << endl;

	int server_coid = -1;

	while (server_coid == -1) {
		if ((server_coid = name_open(REG_CHAN, 0)) == -1) {
			cout << "Client 	: server connection ERROR" << endl;
			//		exit(EXIT_FAILURE);
		}
		sleep(1);
	}

	cout << "Client		: Connection success. Sending data..." << endl;

	MsgSend(server_coid, &msg, sizeof(msg), NULL, 0);

	/* Закрыть соединение с сервером */
	name_close(server_coid);

	while (!stop_thread) {

	}

	return EXIT_SUCCESS;
}

void handler(int sig) {
	if (sig == SIGUSR1) {
		tik_count++;
		cout << "handler	: SIGUSR1 received!!!" << endl;
		//		stop_thread = 1;
	}
}
