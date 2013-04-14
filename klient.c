#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "klient_przesylanie.h"

#include <signal.h>

int ppid;

int main(int argc, char **argv){
int rc;
signal(SIGINT, terminate);
signal(SIGALRM, timeout);
pthread_t threads[3];
ppid=getpid();
polacz();
//watek odpowiedzialny za wysylanie wiadomosci
rc = pthread_create(&threads[0], NULL, wysylaj, (void *)ppid);
if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
}
//w¹tek odpowiedzialny za odbieranie wiadomosci
rc = pthread_create(&threads[0], NULL, odbieraj, (void *)ppid);
if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
}
//watek odpowiedzialny za komunikaty z serwera
rc = pthread_create(&threads[0], NULL, odpowiedzNaKomunikaty, (void *)ppid);
if (rc){
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
}
while(test==1);
/*
if((fork())==0){
	wysylaj(ppid);
	return 0;
}

if((fork())==0){
	odbieraj(ppid);
	return 0;
}
if((fork())==0){
	odpowiedzNaKomunikaty(ppid);
	return 0;
}
wait(NULL);*/
terminate(0);
pthread_exit(NULL);
return 0;
}
