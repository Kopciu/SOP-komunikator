#define checkTime 5
#define idleTimes 5
time_t lastCheck;

//funkcja-petla odpowiedzialna za heartbeat
void maintenance(int ppid);
//wysyla zadanie potwierdzenia aktywnosci do wszystkich serwerow i przetwarza obecny stan odpowiedzi
void checkServers();
//wysyla zadanie potwierdzenia aktywnosci do wszystkich klientow i przetwarza obecny stan odpowiedzi
void checkClients();
//sprawdza, czy serwer nie zostal wyrejestrowany
int selfCheck();

void maintenance(int ppid){
while(getppid()==ppid){
	if(selfCheck()==-1)
		terminate(-1);
	if(time(NULL)-lastCheck<5) continue;
	checkServers();
	checkClients();
	lastCheck=time(NULL);
}
}
int selfCheck(){
int i;
//semafor
opusc();
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]==serverMsgId){
		podnies();
		return 1;
	}
}
podnies();
//semafor
return -1;
}
//semafory
void checkServers(){

if(config[2]==0) return;
int i, status;
pingServer2server pSS;
pSS.mtype=MTYPE_pingServer2server;
pSS.server_id=serverMsgId;
pSS.msgCode=CODE_POLL;
pSS.time=time(NULL);
//semafor
opusc();
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]==servers[i]){
		if(servers[i]==serverMsgId || servers[i]==-1) continue;
		serverPolls[i]--;
		if(serverPolls[i]<0){
			ld.id=servers[i];
			writeToLog(LOG_DELETE_SERVER, ld);
			wyrejestrujSerwerSM(servers[i]);
			continue;
		}
		status=msgsnd(servers[i], &pSS, sizeof(pingServer2server)-sizeof(long), 0);
		if(status==-1)
			wyrejestrujSerwerSM(servers[i]);
	}
	else{
		servers[i]=repo->serversIds[i];
		serverPolls[i]=idleTimes;
	}
}

podnies();
//semafor
}
//semafory
void checkClients(){
if(config[1]==0) return;
int i;
for(i=0; i<MAX_USERS; i++){
	if(ids[i]!=-1)
		if(polls[i]==0){
		strcpy(ld.nick, usersTab[i]);
		writeToLog(LOG_DELETE_USER, ld);
		//semafor
		opuscD();
		usunUseraD(usersTab[i]);
		podniesD();
		//semafor
		}
}
//semafor
opuscD();
decClientPollsD();
podniesD();
//semafor
pingClient pC;
pC.mtype=MTYPE_pingClient;
pC.msgCode=CODE_POLL;
pC.time=time(NULL);
int res;
for(i=0; i<MAX_USERS; i++)
	if(ids[i]!=-1){
		res=msgsnd(ids[i], &pC, sizeof(pingClient)-sizeof(long), 0);
		if(res==-1){
			//semafor
			opuscD();
			usunUseraD(usersTab[i]);
			podniesD();
			//semafor
		}
	}
}
