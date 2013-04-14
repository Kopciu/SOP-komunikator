#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "serwer_przesylanie.h"
#include "serwer_heartbeat.h"
#include "serwer_input.h"
#include <signal.h>




int main(int argc, char **argv){
signal(SIGINT, terminate);
signal(SIGCHLD, terminate);
int ppid=getpid();

postawSerwer();

if((fork())==0){
	odbieranieWiadomosci(ppid);
	return 0;
}

if((fork())==0){
	odbieranieKomunikatowKlienta(ppid);
	return 0;
}
if((fork())==0){
	odbieranieKomunikatowSerwera(ppid);
	return 0;
}
if((fork())==0){
	lastCheck=time(NULL);
	maintenance(ppid);
	return 0;
}
if((fork())==0){
	czytajInput(ppid);
	return 0;
}
wait(NULL);
printf("koniec!\n");
terminate(0);
return 0;
}
