
//przetwarza polecenia dla serwera wpisane z klawiatury
void czytajInput(int ppid);

void czytajInput(int ppid){
char msg[MSG_LENGTH];
while(ppid==getppid()){
	scanf("%s", msg);
	if(ppid!=getppid()) return;
	     if(strcmp(msg, "exit")==0) return;
	else if(strcmp(msg, "me")==0) printf("id serwera: %d\nid semaforow: %d\nid pamieci: %d\nraporty: %d\npollC: %d\npollS: %d\n", serverMsgId, semKey, shmKey, config[0], config[1], config[2]);
	else if(strcmp(msg, "help")==0) printf(serverCommandList);
	else if(strcmp(msg, "localdata")==0){
						//semafor
						opuscD();
						wyswietlDaneD();
						podniesD();
						//semafor
						}
	else if(strcmp(msg, "reportON")==0) config[0]=1;
	else if(strcmp(msg, "reportOFF")==0) config[0]=0;
	else if(strcmp(msg, "servers")==0) wyswietlRepoServersSM();
	else if(strcmp(msg, "users")==0) wyswietlRepoClientsSM();
	else if(strcmp(msg, "channels")==0) wyswietlRepoChannelsSM();
	else if(strcmp(msg, "pollClientON")==0) config[1]=1;
	else if(strcmp(msg, "pollClientOFF")==0) config[1]=0;
	else if(strcmp(msg, "pollServerON")==0) config[2]=1;
	else if(strcmp(msg, "pollServerOFF")==0) config[2]=0;

}
}
