#include <time.h>
#include "constans.h"

typedef struct pingServer{
   long mtype;
   char nick[NICK_LENGTH];
   char message[MSG_LENGTH];
   int msgCode;
   time_t time;
} pingServer;

typedef struct pingServer2Server{
   long mtype;
   int server_id;
   int msgCode;
   time_t time;
} pingServer2server;

typedef struct pingClient{
   long mtype;
   int msgCode;
   time_t time;
} pingClient;

typedef struct message{
   long mtype;
   char sender[NICK_LENGTH];
   char receiver[NICK_LENGTH];
   char message[MSG_LENGTH];
    int klientId; //identyfikator kolejki komunikatów klienta
    int msgCode;
   time_t time;
   int private;
} message;

typedef struct sharedMemory{
	int serversIds[MAX_SERVERS];
	char channels[MAX_CHANNEL*MAX_SERVERS][NICK_LENGTH];
	int channelsCount[MAX_CHANNEL*MAX_SERVERS];
	char clients[MAX_USERS*MAX_SERVERS][NICK_LENGTH];
	int clientsServerId[MAX_CHANNEL*MAX_SERVERS];
} sharedMemory;
