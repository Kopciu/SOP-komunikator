#include <string.h>
#include <sys/shm.h>
#include "constans.h"

int shmid;
char *tab;
char *kanal;
char *oldChannel;

void inicjalizujShm();
void usunShm();

void inicjalizujShm(){
shmid=shmget(IPC_PRIVATE, 2*NICK_LENGTH*sizeof(char), 0777|IPC_CREAT);
if(shmid==-1){ perror("shmem!"); return;}
tab=(char*)shmat(shmid, NULL, 0);
if(tab==NULL){ perror("shmat!"); return;}
kanal=tab;
oldChannel=(tab+NICK_LENGTH);
}
void usunShm(){
shmdt(tab);
shmctl(shmid, IPC_RMID, 0);
}
