#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "structures.h"

int serverMsgId=-1;
int klientMsgId=-1;
char *kanal;
char *oldChannel;
char userNick[NICK_LENGTH];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int testTerm=1;

//odpowiada za zakonczenie pracy programu, jezeli serwer nie odpowiada na logowanie przez 5s
void timeout(int sig);
//konczy dzialanie programu
void terminate(int sig);
//przeprowadza procedure polaczenia z serwerem
void polacz();
//pobiera od nazwe uzytkownika
void getNick();
//pobiera adres na podstawie wyswietlonej wiadomosci message
int getAdres(char* message);
//tworzy kolejke komunikatow klienta
int stworzKolejkeKlienta();
//nawi¹zuje polaczenie z kolejka komunikatow serwera
int polaczZKolejkaSerwera();
//sprzata przed wylaczeniem klienta
int usunKolejkeKlienta();
//przesyla do serwera nazwe usera, po czym przetwarza odpowiedz
int potwierdzNick(char *nick);
//informuje serwer o wylogowaniu siê klienta
int wyloguj();

void timeout(int sig){
	printf("serwer nie odpowiada, connection timeout...\n");
	terminate(-1);
}
void terminate(int sig){
	//semafor
	pthread_mutex_lock(&mutex);
	if(testTerm!=1){
		pthread_mutex_unlock(&mutex);
		//semafor
	 	exit(0);
	}
	testTerm=0;
	pthread_mutex_unlock(&mutex);
	//semafor
	if(sig==1)
		printf("najprawdopodobniej serwer jest offline, koncze dzialanie programu\n");
	wyloguj();
	usunKolejkeKlienta();
	exit(0);
}


void polacz(){
kanal=(char*) malloc(NICK_LENGTH*sizeof(char));
oldChannel=(char*) malloc(NICK_LENGTH*sizeof(char));
klientMsgId=stworzKolejkeKlienta();
if(klientMsgId==-1) terminate(-1);
serverMsgId=polaczZKolejkaSerwera();
getNick();
//czekaj na potwierdzenie
//inicjalizujShm();
strcpy(kanal,GLOBAL_CHANNEL);
system("clear");
printf("%s witaj na kanale %s, wpisz help aby zobaczyc dostepne funkcje\n", userNick, kanal); //ogarnij nazwe kanalu itp

}

int stworzKolejkeKlienta(){
int id=-1;
	id=msgget(IPC_PRIVATE, 0777|IPC_CREAT|IPC_EXCL);
	if(id==-1){
		perror("Nie mozna utworzyc kolejki");
		return -1;
	}
printf("stworzono kolejke!\n");
return id;
}
int polaczZKolejkaSerwera(){
int id=-1;
while(1){
	id=getAdres("Podaj adres serwera");
	if(id!=-1)
		break;
	perror("Nie mozna polaczyc z serwerem");
}
printf("polaczono z serwerem!!\n");
return id;
}

int getAdres(char* message){
printf("%s %s\n", message, "lub podaj -1 aby wyjsc");
int n;
scanf("%d", &n);
if(n==-1){
	printf("wychodze z programu\n");
	usunKolejkeKlienta();
	exit(-1);
}
return n;
}
void getNick(){
char nick[NICK_LENGTH];
int status;
while(1){
	printf("podaj swoj nick, lub -1 aby wyjsc\n");
	scanf("%s", nick);
	printf("sprawdzanie dostepnosci: %s\n", nick);
	alarm(5);
	status=potwierdzNick(nick);
	alarm(0);
	if(status==CODE_LOGGED)
		break;
	else if(status==CODE_NICK_TAKEN)
		printf("nick zajety!\n");
	else if(status==CODE_NO_SPACE)
		printf("brak miejsca na serwerze!\n");
}
strcpy(userNick, nick);
printf("zalogowano!!\n");
}
int potwierdzNick(char *nick){
if(strcmp(nick, "-1")==0) exit(0);
pingServer pS;
pingClient pC;
pS.mtype=MTYPE_pingServer;
strcpy(pS.nick, nick);
strcpy(pS.message, "");
pS.msgCode=klientMsgId;
pS.time=time(NULL);
int status;
status=msgsnd(serverMsgId, &pS, sizeof(pingServer)-sizeof(long), 0);
if(status==-1){terminate(1); }
msgrcv(klientMsgId, &pC, sizeof(pingClient)-sizeof(long), MTYPE_pingClient, 0);
if(pC.msgCode==CODE_LOGGED)
	return CODE_LOGGED;
else if(pC.msgCode==CODE_NICK_TAKEN){
	return CODE_NICK_TAKEN;
}
else if(pC.msgCode==CODE_NO_SPACE){
	return CODE_NO_SPACE;
}
return 0;
}
int wyloguj(){
pingServer pS;
pS.mtype=MTYPE_pingServer;
strcpy(pS.nick, userNick);
strcpy(pS.message, "");
pS.msgCode=CODE_LOGOUT;
pS.time=time(NULL);
return msgsnd(serverMsgId, &pS, sizeof(pingServer)-sizeof(long), 0);
}
int usunKolejkeKlienta(){
int res=0;
if(klientMsgId!=-1){
	res=msgctl(klientMsgId, IPC_RMID, 0);
}
pthread_mutex_destroy(&mutex);
//usunShm();
return res;

}
