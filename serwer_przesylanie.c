#include "serwer_przesylanie.h"

void odbieranieWiadomosci(int ppid){
message m;
int status;
while(ppid==getppid()){
	status=msgrcv(serverMsgId, &m, sizeof(message)-sizeof(long), MTYPE_message, 0);
	if(status==-1){ terminate(-1); }
	//semafor
	opuscD();
	if(getKolejkeUseraD(m.sender)==-1 && msg.msgCode!=CODE_SENDER_SERVER){
		podniesD();
		if(config[0]==1)
			printf("odebrano wiadomość, ale nadawca, %s, jest wyrejestrowany\n", m.sender);
		wyslijPingClient(m.klientId, CODE_WRONG_RECEIVER);
		continue;
	}
	podniesD();
	//semafor
	odbierz(m); 
}
}

void odbieranieKomunikatowSerwera(int ppid){
int status, receiverId;
pingServer2server pSS;
while(ppid==getppid()){
	status=msgrcv(serverMsgId, &pSS, sizeof(pingServer2server)-sizeof(long), MTYPE_pingServer2server, 0);
	if(status==-1){ terminate(-1); }
	if(pSS.msgCode==CODE_POLL){
		if(config[0]==1)
			printf("otrzymano rządanie potwierdzenia aktywności serwera od: %d\n", pSS.server_id);
		continue;
		receiverId=pSS.server_id;
		pSS.server_id=serverMsgId;
		pSS.msgCode=CODE_SERVER_OK;
		pSS.time=time(NULL);
		status=msgsnd(receiverId, &pSS, sizeof(pingServer2server)-sizeof(long), 0);
		if(status==-1){ terminate(-1); }
	}
//semafory
	else if(pSS.msgCode==CODE_SERVER_OK){
		if(config[0]==1)
			printf("otrzymano potwierdzenie aktywności od serwera %d\n", pSS.server_id);
		int i;
		for(i=0; i<MAX_SERVERS; i++)
			if(servers[i]==pSS.server_id && pSS.server_id!=-1){
				//semafor
				opuscD();
				serverPolls[i]=idleTimes;
				opuscD();
				//semafor
				break;
			}
		

	}
}

}
void odbierz(message msg){
//temporary
if(config[0]==1)
	printf("odebrano wiadomość: [%s]->[%s]: %s\n", msg.sender, msg.receiver, msg.message);
if(msg.private==PRIV_CH)
	wyslijDoWszystkichNaKanale(msg);
else{
	
	//semafor
	opuscD();
	int id=getKolejkeUseraD(msg.receiver);
	podniesD();
	//semafor
	if(id==-1){
		//semafor
		opusc();
		id=getSerwerMacierzystySM(msg.receiver);
		podnies();
		//semafor
		if(id==-1){
			//semafor
			opuscD();
			wyslijPingClient(getKolejkeUseraD(msg.sender), CODE_WRONG_RECEIVER);
			podniesD();
			//semafor
			return;
		}
		else{
		msg.msgCode=CODE_SENDER_SERVER;
		}
	}
	int status=msgsnd(id, &msg, sizeof(message)-sizeof(long), 0);
	if(status==-1){
		msg.msgCode=CODE_FAILED_TO_DELIVER;
		char tmp[NICK_LENGTH];
		strcpy(tmp, msg.sender);
		strcpy(msg.sender, msg.receiver);
		strcpy(msg.receiver, tmp);
		status=msgsnd(serverMsgId, &msg, sizeof(message)-sizeof(long), 0);
		 return;
	}
}
}
//dostep do wspoldzielonej! bledy!
void wyslijDoWszystkichNaKanale(message msg){
if(msg.private!=PRIV_CH) return;
char *channel=msg.receiver;
int i;
for(i=0; i<MAX_USERS; i++){
	if(strcmp(channelsTab[i], channel)==0 && strcmp(usersTab[i], msg.sender)!=0){
		msgsnd(ids[i], &msg, sizeof(message)-sizeof(long), 0);
	}
}
if(msg.msgCode==CODE_SENDER_SERVER) return;
msg.msgCode=CODE_SENDER_SERVER;
//semafor
opusc();
for(i=0; i<MAX_SERVERS; i++)
	if(repo->serversIds[i]!=-1 && repo->serversIds[i]!=serverMsgId)
		msgsnd(repo->serversIds[i], &msg, sizeof(message)-sizeof(long), 0);
podnies();
//semafor
}

void odbieranieKomunikatowKlienta(int ppid){
pingServer pS;
//pingClient pC;
int status;
while(ppid==getppid()){
	status =msgrcv(serverMsgId, &pS, sizeof(pingServer)-sizeof(long), MTYPE_pingServer, 0);
	if(status==-1){ terminate(-1);}
	//ogarnij rozne wiadomosci
	if(pS.msgCode>=0){
		
		//semafor
		opusc();
		if(zarejestrujKlientaSM(pS.nick, serverMsgId)==CODE_LOGGED){
			podnies();
			//semafor
			opuscD();
			status=dodajUseraD(pS.nick, pS.msgCode);
			podniesD();			
			//semafor
			if(status==CODE_LOGGED){
				logData ld;
				ld.nick=pS.nick;
				writeToLog(LOG_ADD_USER, ld);
			}
		}
		else
			podnies();
		//semafor
		if(status==CODE_LOGGED && config[0]==1){
			printf("zarejestrowano usera: %s ", pS.nick);
			printf("Status: %d\n", status);
		}
		wyslijPingClient(pS.msgCode, status);
		
	}
	else if(pS.msgCode==CODE_JOIN_CH){
		int status;
		if(config[0]==1)
			printf("Polecenie zmiany kanału: user %s, kanał %s\n", pS.nick, pS.message);
		//semafor
		opusc();
		opuscD();
		zabierzZKanaluSM(getKanalUseraD(pS.nick));
		podniesD();
		podnies();
		//semafor
		//semafor
		opuscD();
		status=setKanalUseraD(pS.nick, pS.message);
		podniesD();
		//semafor
	
		if(status==-1){
			wyslijPingClient(getKolejkeUseraD(pS.nick), CODE_NO_SPACE);
			return;
		}
		//semafor
		opusc();
		status=dodajDoKanaluSM(pS.message);
		podnies();
		//semafor
		if(status==-1){
			//semafor
			opuscD();
			wyslijPingClient(getKolejkeUseraD(pS.nick), CODE_NO_SPACE);
			podniesD();
			//semafor
			return;
		}
		//semafor
		opuscD();
		wyslijPingClient(getKolejkeUseraD(pS.nick), CODE_LOGGED);
		podniesD();
		//semafor

	}
	else if(pS.msgCode==CODE_LOGOUT){
		if(config[0]==1)
			printf("otrzymano polecenie wylogowania klienta: %s\n", pS.nick);

		usunKlienta(pS.nick);

	}
	else if(pS.msgCode==CODE_CLIENT_OK){
		if(config[0]==1)
			printf("otrzymano potwierdzenie aktywności klienta: %s\n", pS.nick);
		//semafor
		opuscD();
		potwierdzAktywnoscD(pS.nick);
		podniesD();
		//semafor
	} //wspoldzielona! bledy!
	else if(pS.msgCode==CODE_REQUEST_U_LIST || pS.msgCode==CODE_REQUEST_CH_LIST ){

		message *m=(message*) malloc(sizeof(message));
		m->mtype=MTYPE_message;
		strcpy(m->sender, SERVER_NAME);
		m->msgCode=CODE_SENDER_SERVER;
		m->time=time(NULL);
		m->private=PRIV_U;
		//semafor
		opuscD();
		int id=getKolejkeUseraD(pS.nick);
		podniesD();
		//semafor
		if(id==-1) continue;
		int i;
		if(pS.msgCode==CODE_REQUEST_U_LIST){
		if(config[0]==1)
			printf("otrzymano rządanie listy userów, klient: %s\n", pS.nick);
		for(i=0; i<MAX_SERVERS*MAX_USERS; i++)
			if(strcmp(repo->clients[i], "")!=0 && repo->clientsServerId[i]!=-1 ){
				strcpy(m->message, repo->clients[i]);
				status=msgsnd(id, m, sizeof(message)-sizeof(long), 0);
			}

		}
		else if(pS.msgCode==CODE_REQUEST_CH_LIST){
		if(config[0]==1)
			printf("otrzymano rządanie listy kanalow, klient: %s\n", pS.nick);
		for(i=0; i<MAX_SERVERS*MAX_CHANNEL; i++)
			if(strcmp(repo->channels[i], "")!=0 && repo->channelsCount[i]!=0 ){
				strcpy(m->message, repo->channels[i]);
				status=msgsnd(id, m, sizeof(message)-sizeof(long), 0);
			}

		}

	}
	
}
}
//semafory	
void usunKlienta(char *nick){
//semafor
opusc();
wyrejestrujKlientaSM(nick);
podnies();
//semafor
//semafor
opuscD();
usunUseraD(nick);
podniesD();
//semafor
}
//ogarnianie bledu
void wyslijPingClient(int id, int code){
int status;
pingClient pC;
pC.mtype=MTYPE_pingClient;
pC.msgCode=code;
pC.time=time(NULL);
status=msgsnd(id, &pC, sizeof(pingClient)-sizeof(long), 0);
if(status==-1){ return; }

}
