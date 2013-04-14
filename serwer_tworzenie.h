#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "serwer_dane.h"
#include "structures.h"
#include "serwer_repo.h"

int serverMsgId=-1;
//inicjalizuje serwer
int postawSerwer();
//tworzy kolejke komunikatow serwera
int stworzKolejkeSerwera();
//sprzata przed usunieciem serwera
int usunKolejkeSerwera();
//konczy dzialanie programu
void terminate(int sig);

//semafory!
void terminate(int sig){
	//semafor
	if(sem==-1){
		usunKolejkeSerwera();
		exit(0);
	}
	opuscD();
	if(config[3]!=0){
		exit(0);
		podniesD();
		//semafor
	}
	config[3]=1;
	podniesD();
	//semafor
	if(sig==-1)
		printf("Serwer napotkal problemy z lacznoscia, zamykam program\n");
	usunKolejkeSerwera();
	exit(0);
}

int postawSerwer(){
//ogarnainei pamieci wspoldzielonej
serverMsgId=stworzKolejkeSerwera();
if(serverMsgId==-1) terminate(-1);
//tmp
printf("podaj Id semaforow\n");
scanf("%d", &semKey);
printf("podaj Id pamięci wspoldzielonej\n");
scanf("%d", &shmKey);
if(-1==podlaczDoObszaruSM(shmKey)){
	podnies();
	perror("shmem");
	terminate(-1);
}
podnies();
//semafor
printf("pamiec podlaczona!\n");
//semafor
opusc();
if(zarejestrujSerwerSM(serverMsgId)==-1){
	podnies();
	printf("nie ma wolnego miejsca na serwer!\n");
	terminate(-1);
}
podnies();
//semafor
//semafor
inicjalizujDaneD();
podniesD();
//semafor
printf("serwer gotowy, wpisz help aby poznac dostepne funkcje\n");
return serverMsgId;
}

int stworzKolejkeSerwera(){
int id=-1;
id=msgget(IPC_PRIVATE, 0777|IPC_CREAT|IPC_EXCL);
if(id==-1){
	perror("Nie mozna utworzyc kolejki");
	return -1;
}
printf("stworzono kolejke! Adres serwera to: %d\n", id);
return id;
}
int usunKolejkeSerwera(){
if(serverMsgId!=-1){
	msgctl(serverMsgId, IPC_RMID, 0);
}
if(shmId==-1 || sem==-1) return 0;
semctl(semD ,0,IPC_RMID);
//semafor
opusc();
wyrejestrujSerwerSM(serverMsgId);
podnies();
//semafor
//skasuj semafory



shmdt(tab);
shmdt(ids);
shmdt(polls);
shmdt(chan);
shmdt(config);
shmdt(serverPolls);
shmdt(servers);
shmctl(shmid, IPC_RMID, 0);
shmctl(shmid2, IPC_RMID, 0);
shmctl(shmid3, IPC_RMID, 0);
shmctl(shmid4, IPC_RMID, 0);
shmctl(shmid5, IPC_RMID, 0);
shmctl(shmid6, IPC_RMID, 0);
shmctl(shmid7, IPC_RMID, 0);
//semafor

opusc();
if(isEmptySM()==1){
	shmdt(repo);
	semctl(sem ,0,IPC_RMID);
	shmctl(shmId, IPC_RMID, 0);
}
else{
	shmdt(repo);
	podnies();
}
//semafor
return 0;

}
