
//funkcje odpowiedzialne za zarządzanie repozytorium
typedef union logData{
	int id;
	char *nick;
} logData;

#define LOG_ADD_USER 10
#define LOG_REMOVE_USER 11
#define LOG_ADD_SERVER 12
#define LOG_REMOVE_SERVER 13
#define LOG_DELETE_USER 14
#define LOG_DELETE_SERVER 15

sharedMemory *repo=NULL;
int shmId=-1;
int semKey, shmKey;
int sem=-1;
logData ld;

//dolacza serwer do repozytorium-obszaru wspolnej pamieci wspoldzielonej, przyjmuje argument shmKey-klucz do SM
int podlaczDoObszaruSM(int shmKey);
//inicjalizuje i wypelnia domyslnymi wartosciami obszar SM, przyjmuje w argumencie jego identyfikator
int stworzObszarSM(int shmId);
//wpisuje serwer o danym Id do SM
int zarejestrujSerwerSM(int id);
//wypisuje serwer o danym ID z SM(i wszystkich klientow podlaczonych do niego)
void wyrejestrujSerwerSM(int id);
//wpisuje klienta o danym nicku, przypisanego do serwera o id=serwerId do SM
int zarejestrujKlientaSM(char *nick, int serwerId);
//wypisuje klienta o danym nicku z SM
void wyrejestrujKlientaSM(char *nick);
//zwieksza liczbe userow przypisanych do kanalu o nazwie name
int dodajDoKanaluSM(char *name);
//zmniejsza liczbe userow przypisanych do kanalu o nazwie name
void zabierzZKanaluSM(char *name);
//zwraca id serwera macierzystego klienta
int getSerwerMacierzystySM(char *nick);
//zwraca 1, jezeli w SM nie ma zarejestrowanych zadnych serwerow
int isEmptySM();

//umieszcza w logu wpis typu op na podstawie danej z ld
void writeToLog(int op, logData ld);
//opuszcza semafor odpowiedzialny za SM
void opusc();
//podnosi semafor odpowiedzialny za SM
void podnies();

void wyswietlRepoServersSM();
void wyswietlRepoClientsSM();
void wyswietlRepoChannelsSM();

int isEmptySM(){
int i;
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]!=-1)
		return 0;
}
return 1;
}

//bledy!
int podlaczDoObszaruSM(int shmKey){

sem=semget(semKey, 2, IPC_CREAT|IPC_EXCL|0777);
if(sem==-1)
	sem=semget(semKey, 2, 0);
else{
	union semun sop;
	sop.val=0;
	semctl(sem, 0, SETVAL, sop);
	sop.val=1;
	semctl(sem, 1, SETVAL, sop);
}
shmId=shmget(shmKey, sizeof(sharedMemory), IPC_CREAT|IPC_EXCL|0777);
if(shmId!=-1){
	stworzObszarSM(shmId);
	}
else{
	shmId=shmget(shmKey, sizeof(sharedMemory), 0);
	repo=(sharedMemory*) shmat(shmId, NULL, 0);
	}

return shmId;
}
int stworzObszarSM(int shmId){

if(shmId==-1){
	perror("blad pamieci wspoldzielonej!");
	return shmId;
}
printf("stworzono pamiec!\n");
repo=(sharedMemory*) shmat(shmId, NULL, 0);
int i;
for(i=0; i<MAX_SERVERS; i++){
	repo->serversIds[i]=-1;
}
for(i=0; i<MAX_CHANNEL*MAX_SERVERS; i++){
	strcpy(repo->channels[i], "");
	repo->channelsCount[i]=0;
}
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	strcpy(repo->clients[i], "");
	repo->clientsServerId[i]=-1;
}
return shmId;
}
void wyswietlRepoServersSM(){
int i;
printf("id serwerow:\n");
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]!=-1)
		printf("%d: %d\n", i, repo->serversIds[i]);
}
}
void wyswietlRepoChannelsSM(){
int i;
printf("kanaly:\n");
for(i=0; i<MAX_CHANNEL*MAX_SERVERS; i++){
	if(strcmp(repo->channels[i], "")!=0)
		printf("%s, %d\n", repo->channels[i], repo->channelsCount[i]);
}
}
void wyswietlRepoClientsSM(){
int i;
printf("klienci i ich serwery:\n");
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(strcmp(repo->clients[i], "")!=0)
		printf("%s at %i\n" ,repo->clients[i], repo->clientsServerId[i]);
}

}


int zarejestrujSerwerSM(int id){
int i;
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]==-1){
		repo->serversIds[i]=id;
		ld.id=id;
		writeToLog(LOG_ADD_SERVER,ld);
		return 1;
	}
}
return -1;
}
void wyrejestrujSerwerSM(int id){
int i;
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(repo->clientsServerId[i]==id){
		repo->clientsServerId[i]=-1;
		strcpy(repo->clients[i], "");
	}
}
for(i=0; i<MAX_SERVERS; i++){
	if(repo->serversIds[i]==id){
		repo->serversIds[i]=-1;
		ld.id=id;
		writeToLog(LOG_REMOVE_SERVER, ld);
	}
}
}


int zarejestrujKlientaSM(char *nick, int serwerId){
int i;
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(strcmp(repo->clients[i], nick)==0)
		return CODE_NICK_TAKEN;
}
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(repo->clientsServerId[i]==-1){
		strcpy(repo->clients[i], nick);
		repo->clientsServerId[i]=serwerId;
		dodajDoKanaluSM(GLOBAL_CHANNEL);
		return 1;
	}
}

return CODE_NO_SPACE;
}

void wyrejestrujKlientaSM(char *nick){
//semafor
opuscD();
zabierzZKanaluSM(getKanalUseraD(nick));
podniesD();
//semafor
int i;
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(strcmp(repo->clients[i], nick)==0){
		strcpy(repo->clients[i], "");
		repo->clientsServerId[i]=-1;
	}
}
ld.nick=nick;
writeToLog(LOG_REMOVE_USER, ld);
}

int dodajDoKanaluSM(char *name){
int i;
for(i=0; i<MAX_CHANNEL*MAX_SERVERS; i++){
	if(strcmp(repo->channels[i], name)==0){
		repo->channelsCount[i]=repo->channelsCount[i]+1;
		return 1;
	}
}
for(i=0; i<MAX_CHANNEL*MAX_SERVERS; i++){
	if(strcmp(repo->channels[i], "")==0 && repo->channelsCount[i]==0){
		repo->channelsCount[i]=1;
		strcpy(repo->channels[i], name);
		return 1;
	}
}
return -1;
}
void zabierzZKanaluSM(char *name){
if(strcmp(name, "")==0) return;
int i;
for(i=0; i<MAX_CHANNEL*MAX_SERVERS; i++){
	if(strcmp(repo->channels[i], name)==0){
		repo->channelsCount[i]--;
		if(repo->channelsCount[i]==0)
			strcpy(repo->channels[i], "");
		return;
	}
}


}

int getSerwerMacierzystySM(char *nick){
int i;
for(i=0; i<MAX_USERS*MAX_SERVERS; i++){
	if(strcmp(repo->clients[i], nick)==0){
		return repo->clientsServerId[i];
	}
}
return -1;
}
void writeToLog(int op, logData ld){

struct sembuf operation;
operation.sem_num=1;
operation.sem_op=-1;
if(semop(sem, &operation, 1)!=0){raise(SIGINT); }

FILE *file =fopen(LOG_FILENAME, "a");
if(file==NULL){
	perror("Nie udało się otworzyć pliku!");
	return;
}
time_t t=time(NULL);
struct tm tm = *localtime(&t);
switch(op){
	case LOG_ADD_USER: fprintf(file, "[%d:%02d:%02d]zarejestrowano klienta: %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.nick); break;
	case LOG_REMOVE_USER: fprintf(file, "[%d:%02d:%02d]wyrejestrowano klienta: %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.nick); break;
	case LOG_DELETE_USER: fprintf(file, "[%d:%02d:%02d]usunięto nieaktywnego klienta: %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.nick); break;
	case LOG_ADD_SERVER: fprintf(file, "[%d:%02d:%02d]zarejestrowano serwer: %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.id); break;
	case LOG_REMOVE_SERVER: fprintf(file, "[%d:%02d:%02d]wyrejestrowano serwer: %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.id); break;
	case LOG_DELETE_SERVER: fprintf(file, "[%d:%02d:%02d]usunięto nieaktywny serwer: %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, ld.id); break;
}
fclose(file);
operation.sem_op=1;
if(semop(sem, &operation, 1)!=0){raise(SIGINT); }
}

void opusc(){
//return;
struct sembuf operation;
operation.sem_num=0;
operation.sem_flg=SEM_UNDO;
operation.sem_op=-1;
if(semop(sem, &operation, 1)!=0){raise(SIGINT); }

}
void podnies(){
//return;
union semun sop;
sop.val=1;
if(semctl(sem, 0, SETVAL, sop)==-1){ 
	semctl(sem, 0, SETVAL, sop);
}
/*
struct sembuf operation;
operation.sem_num=0;
operation.sem_op=1;
if(semop(sem, &operation, 1)!=0){ perror("semPod"); }//raise(SIGINT); }*/
}
