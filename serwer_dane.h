#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "constans.h"

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
#include <sys/sem.h>
#else
union semun {
      int val;                  /* value for SETVAL */
      struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
      unsigned short *array;    /* array for GETALL, SETALL */
                                /* Linux specific part: */
      struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif


char *tab;
char **usersTab;
int *ids;
int *polls;
int shmid, shmid2, shmid3;

int shmid4; //kanaly
char **channelsTab, *chan;

int shmid5; //opcje
int *config;

int shmid6, shmid7; //odpytywanei serwerow
int *serverPolls, *servers;
int semD;
//inicjalizuje prywatny segment pamieci wspoldzielonej serwera
void inicjalizujDaneD();
//rejestruje uzytkownika
int dodajUseraD(char *nick, int id);
//usuwa uzytkownika
void usunUseraD(char *nick);
//zwraca id kolejki danego uzytkownika
int getKolejkeUseraD(char *nick);
//zwraca nazwe kanalu, na ktorym jest dany uzytkownik
char *getKanalUseraD(char *nick);
//ustawia kanal,m an ktorym jest teraz dany uzytkownik
int setKanalUseraD(char *nick, char *channel);
//obniza ilosc heartbeatow, ktore moze zignorowac kazdy z uzytkownikow
void decClientPollsD();
//void decServerPollsD();
//wyswietla lokanlen dane o uzytkownikach
void wyswietlDaneD();
//potwierdza odpoweidz na heartbeat danego uzytkownika
void potwierdzAktywnoscD(char *nick);

void opuscD();
void podniesD();

void inicjalizujDaneD(){
semD=semget(IPC_PRIVATE, 1, IPC_CREAT|0700);
union semun sop;
sop.val=0;
semctl(semD, 0, SETVAL, sop);

//ogarnianie bledu
shmid=shmget(IPC_PRIVATE, MAX_USERS*NICK_LENGTH*sizeof(char), 0777|IPC_CREAT);
if(shmid==-1){ perror("shmem!"); return;}
tab=(char*)shmat(shmid, NULL, 0);
if(tab==NULL){ perror("shmat!"); return;}

usersTab=(char**) malloc(MAX_USERS*sizeof(char*));
int i;
for(i=0; i<MAX_USERS; i++)
	usersTab[i]=tab+i*NICK_LENGTH;

shmid2=shmget(IPC_PRIVATE, MAX_USERS*sizeof(int), 0777|IPC_CREAT);
ids=shmat(shmid2, NULL, 0);

shmid3=shmget(IPC_PRIVATE, MAX_USERS*sizeof(int), 0777|IPC_CREAT);
polls=shmat(shmid3, NULL, 0);


shmid4=shmget(IPC_PRIVATE, MAX_USERS*NICK_LENGTH*sizeof(char), 0777|IPC_CREAT);
if(shmid4==-1){ perror("shmem!"); return;}
chan=(char*)shmat(shmid4, NULL, 0);
if(tab==NULL){ perror("shmat!"); return;}
channelsTab=(char**) malloc(MAX_USERS*sizeof(char*));

for(i=0; i<MAX_USERS; i++)
	channelsTab[i]=chan+i*NICK_LENGTH;

for(i=0; i<MAX_USERS; i++){
	strncpy(usersTab[i], "", NICK_LENGTH);
	strncpy(channelsTab[i], "", NICK_LENGTH);
	ids[i]=-1;
	polls[i]=-1;
}
shmid5=shmget(IPC_PRIVATE, 4*sizeof(int), 0777|IPC_CREAT);
if(shmid5==-1){ perror("shmem!"); return;}
config=(int*)shmat(shmid5, NULL, 0);
if(config==NULL){ perror("shmat!"); return;}
config[0]=1;
config[1]=config[2]=config[3]=0;

shmid6=shmget(IPC_PRIVATE, MAX_SERVERS*sizeof(int), 0777|IPC_CREAT);
serverPolls=(int*)shmat(shmid6, NULL, 0);
shmid7=shmget(IPC_PRIVATE, MAX_SERVERS*sizeof(int), 0777|IPC_CREAT);
servers=(int*)shmat(shmid7, NULL, 0);
}


int dodajUseraD(char *nick, int queue){
//ogarnij bledy
if(queue==-1){ perror("blad!"); return 0; }
int i;
for(i=0; i<MAX_USERS; i++){
if(strcmp(usersTab[i], nick)==0)
		return CODE_NICK_TAKEN;
}
for(i=0; i<MAX_USERS; i++){
	if(ids[i]==-1){
		ids[i]=queue;
		strcpy(usersTab[i], nick);
		strcpy(channelsTab[i], GLOBAL_CHANNEL);
		polls[i]=5;
		//wyswietlDane();
		return CODE_LOGGED;
	}
}
//wyswietlDane();
return CODE_NO_SPACE;
}
void usunUseraD(char *nick){
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(usersTab[i], nick)==0){
		ids[i]=-1;
		polls[i]=1;
		strcpy(usersTab[i], "");
		strcpy(channelsTab[i], "");
		//wyswietlDane();
		return;
	}
}
}
int getKolejkeUseraD(char *nick){
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(usersTab[i], nick)==0){
		return ids[i];
	}
}
return -1;
}
int setKanalUseraD(char *nick, char *channel){
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(usersTab[i], nick)==0){
		strcpy(channelsTab[i], channel);
		//wyswietlDane();
		return 1;
	}
}
return -1;
}

char *getKanalUseraD(char *nick){
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(usersTab[i], nick)==0){
		return channelsTab[i];
	}
}
return "";
}

void decClientPollsD(){
int i;
for(i=0; i<MAX_USERS; i++){
	if(ids[i]!=-1){
		polls[i]--;
		if(polls[i]<0)
			polls[i]=0;
		}
}
}
void potwierdzAktywnoscD(char *nick){
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(usersTab[i], nick)==0){
		polls[i]=5;
	}
}
}
void wyswietlDaneD(){
printf("userzy:\n");
int i;
for(i=0; i<MAX_USERS; i++){
	if(ids[i]!=-1)
		printf("[%s][%s]: %d %d\n", channelsTab[i], usersTab[i], ids[i], polls[i]);
}
}
void opuscD(){
//return;
struct sembuf operation;
operation.sem_num=0;
operation.sem_op=-1;
if(semop(semD, &operation, 1)!=0){raise(SIGINT); }
}
void podniesD(){
//return;
struct sembuf operation;
operation.sem_num=0;
operation.sem_op=1;
if(semop(semD, &operation, 1)!=0){raise(SIGINT); }
}
