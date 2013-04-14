#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>
#include "klient_polacz.h"

//funkcja-petla pobierajaca z klawiatury, przetwarzajaca i wysylajaca wiadomosci
void *wysylaj(void* ppid);
//funkcja odczytujaca i przetwarzajaca otrzymane wiadomosci i zwracajaca je
message* odczytajWiadomosc();
//funkcja-petla odpowiedzialna za odbieranie wiadomosci
void *odbieraj(void* ppid);
//funkcja-petla odpowiedzialna za komunikacje z serwerem
void *odpowiedzNaKomunikaty(void* ppid);
//wysyla do serwera strukture pingServer o danym kodzie
void wyslijPingSerwer(int code);
//wysyla do serwera strukture pingServer o danym kodzie i z dana wiadomoscia
void wyslijPingSerwerM(int code, char *message);

int test=1;

void *wysylaj(void* ppid){

int stat;
message *m;
while(1){
	m=odczytajWiadomosc();
	if(m==NULL) continue;
	stat=msgsnd(serverMsgId, m, sizeof(message)-sizeof(long), 0);
	struct tm tm = *localtime( &m->time );
	if(m->private==PRIV_U)
		printf("[%d:%02d:%02d][%s][private]:%s\n",tm.tm_hour, tm.tm_min, tm.tm_sec, m->sender, m->message);
	else
   		printf("[%d:%02d:%02d][%s][%s]:%s\n",tm.tm_hour, tm.tm_min, tm.tm_sec, m->sender, kanal, m->message);
	if(stat==-1){
	printf("Nie udalo sie wyslac wiadomosci z [%d:%d:%d]\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
	terminate(1);
	break;
	}
}
test--;
pthread_exit(NULL);
}

message* odczytajWiadomosc(){
char msg[MSG_LENGTH];
message *m=(message*) malloc(sizeof(message));
scanf("%s", msg);
if(strcmp(msg, "exit")==0) terminate(0);
else if(strcmp(msg, "help")==0){ printf(commandsList); return NULL; }
else if(strcmp(msg, "users")==0){wyslijPingSerwer(CODE_REQUEST_U_LIST); return NULL;}
else if(strcmp(msg, "channels")==0){wyslijPingSerwer(CODE_REQUEST_CH_LIST); return NULL;}
//semafor
else if(strcmp(msg, "channel")==0){pthread_mutex_lock(&mutex); printf("obecny kanal: %s\n", kanal); pthread_mutex_unlock(&mutex); return NULL;}
//semafor
else if(strcmp(msg, "goto")==0){
	//semafor
	pthread_mutex_lock(&mutex);
	strcpy(oldChannel, kanal);
	scanf("%s", kanal);
	pthread_mutex_unlock(&mutex);
	//semafor
	wyslijPingSerwerM(CODE_JOIN_CH, kanal);
	return NULL;
}
m->mtype=MTYPE_message;
strcpy(m->sender, userNick);
m->msgCode=CODE_SENDER_CLIENT;
m->time=time(NULL);
if(strcmp(msg, "priv")==0){
	scanf("%s", m->receiver);
	gets(m->message);
	m->private=PRIV_U;
}
else{
	m->private=PRIV_CH;
	//semafor
	pthread_mutex_lock(&mutex);
	strcpy(m->receiver, kanal);
	pthread_mutex_unlock(&mutex);
	//semafor
	char tmp[MSG_LENGTH];
	gets(tmp);
	strcat(msg, tmp);
	strcpy(m->message, msg);
}
return m;
}

void *odbieraj(void* ppid){
message m;
int status;
while(1){
	status=msgrcv(klientMsgId, &m, sizeof(message)-sizeof(long), MTYPE_message, 0);
	if(status==-1){ terminate(1); }
   	struct tm tm = *localtime( &m.time );
	if(m.msgCode==CODE_FAILED_TO_DELIVER){
		printf("[failed to deliver!]: %s\n", m.message);
		}
	if(m.private==PRIV_U)
		printf("[%d:%02d:%02d][%s][private]:%s\n",tm.tm_hour, tm.tm_min, tm.tm_sec, m.sender, m.message);
	else
   		printf("[%d:%02d:%02d][%s][%s]:%s\n",tm.tm_hour, tm.tm_min, tm.tm_sec, m.sender, kanal, m.message);
}
test--;
pthread_exit(NULL);
}
void *odpowiedzNaKomunikaty(void* ppid){
pingClient m;
int status;
while(1){
	status=msgrcv(klientMsgId, &m, sizeof(pingClient)-sizeof(long), MTYPE_pingClient, 0);
	if(status==-1){ terminate(1); }
	//ogarnij dodatkowe rzeczy
	if(m.msgCode==CODE_POLL){
		wyslijPingSerwer(CODE_CLIENT_OK);
	}
	else if(m.msgCode==CODE_WRONG_RECEIVER){
		printf("Wiadomość nie doszła-zly adresat\n");
	}
	else if(m.msgCode==CODE_LOGGED){
			//semafor
			pthread_mutex_lock(&mutex);
			printf("obecny kanal: %s\n", kanal);
			pthread_mutex_unlock(&mutex);
			//semafor
		}
	else if(m.msgCode==CODE_NO_SPACE){
		//semafor
		pthread_mutex_lock(&mutex);
		printf("nie udalo sie podlaczyc do: %s\n", kanal);
		strcpy(kanal, oldChannel);
		pthread_mutex_unlock(&mutex);
		//semafor
	}
	else if(m.msgCode==CODE_SENDER_OFF){
		printf("klient został wyrejestrowany, musisz sie ponownie zalogowac. Koncze dzialanie programu\n");
		terminate(0);
	}
}
test--;
pthread_exit(NULL);
}
void wyslijPingSerwer(int code){
int status;
pingServer pS;
pS.mtype=MTYPE_pingServer;
strcpy(pS.nick, userNick);
strcpy(pS.message, "");
pS.msgCode=code;
pS.time=time(NULL);
status=msgsnd(serverMsgId, &pS, sizeof(pingServer)-sizeof(long), 0);
if(status==-1){  terminate(1); }
}
void wyslijPingSerwerM(int code, char *message){
int status;
pingServer pS;
pS.mtype=MTYPE_pingServer;
strcpy(pS.nick, userNick);
strcpy(pS.message, message);
pS.msgCode=code;
pS.time=time(NULL);
status=msgsnd(serverMsgId, &pS, sizeof(pingServer)-sizeof(long), 0);
if(status==-1){ terminate(1); }

}

