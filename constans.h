#define NICK_LENGTH 32
#define MSG_LENGTH 1024
#define MAX_SERVERS 20
#define MAX_USERS 20
#define MAX_CHANNEL 20
#define MAX_SERVER 20

#define SERVER_NAME "serwer"
#define GLOBAL_CHANNEL "global"
#define LOG_FILENAME "/tmp/czat.log"

//mtypy
#define MTYPE_pingServer 2013001
#define MTYPE_pingServer2server 2013002
#define MTYPE_pingClient 2013003
#define MTYPE_message 2013004
#define MTYPE_messageServer2Client 2013005

//pingServer msgCode
#define CODE_LOGOUT -1	//wyrejestrowanie się
#define CODE_CLIENT_OK -2	//pozytywna odpowieź na heartbeat serwera
#define CODE_JOIN_CH -3	//chęć dołączenia do konkretnego kanału
#define CODE_REQUEST_U_LIST -10	//żądanie listy użytkowników
#define CODE_REQUEST_CH_LIST -11	//żądanie listy kanałów do wyboru.


//pingServer2server msgCode
#define CODE_POLL 0	//żądanie odpowiedzi na heartbeat
#define CODE_SERVER_OK 1	//pozytywna odpowiedź na heartbeat

//pingClient msgCode
#define CODE_POLL 0	//wykrywanie aktywności-heartbeat
#define CODE_LOGGED 1	//potwierdzenie zalogowania
#define CODE_NICK_TAKEN -1	//nick zajęty
#define CODE_WRONG_RECEIVER -2	//nieprawidłowy adresat wiadomości
#define CODE_SENDER_OFF -3	//klient-nadawca został wyrejestrowany (np przez nieodpowiadanie)
#define CODE_NO_SPACE -4	//brak wolnych miejsc
#define CODE_NOT_IN_ROOM -10	//klient nie podłączony do żadnego kanału/pokoju

//message
#define CODE_SENDER_CLIENT 0	//wiadomość wysłana przez klienta;
#define CODE_SENDER_SERVER 1	//wiadomość przekazana dalej przez serwer
#define CODE_FAILED_TO_DELIVER -1	//nie udało się dostarczyć tej konkretnej wiadomości.

#define PRIV_CH 1	//tak, odbiorcą jest kanał
#define PRIV_U 3	//tak,odbiorcą jest inny konkretny użytkownik

//changelog: zmienienie stalych w pingSerwer na ujemne
//changelog: dodanei do ping client CODE_LOGGED=1
//changelog: dodanei stalej SERVER_NAME="serwer"
//changelog: dodanie stalej MAX_SERVER=20
//changelog: MAX_CHANEL->MAX_CHANNEL=20
//changelog: usuniecie messageServer2Client
//changelog: usuniecie PRIV_NO
//changelog: usuniecie CODE_LEAVE_CH
//changelog: dodanie stalej GLOBAL_CHANNEL
//changelog: dodanie stalej LOG_FILENAME
//changelog: dodanie pola klientId do struktury message



#define commandsList " channels - pobiera i wyswietla liste dostepnych kanalow\n exit - konczy dzialanie programu\n goto /nazwa/ - przechodzi do kanalu o nazwie /nazwa/\n help - wyswietla dostepne komendy\n priv /nick/ - wysyla wiadomosc prywatna do usera nick\n users - pobiera i wyswietla liste dostepnych uzytkownikow\n"

#define serverCommandList " channels - wyswietla liste dostepnych kanalow\n exit - konczy dzialanie programu\n help - wyswietla dostepne komendy\n localdata - wyswietla dane klientow na serwerze\n me - wyswietla numery kolejek serwera\n pollClient[ON/OFF] - wlacza/wylacza odpytywanie klientow\n pollServer[ON/OFF] - wlacza/wylacza odpytywanie serwerow\n report[ON/OFF] - wlacza/wylacza wypisywanie dzialan serwera\n servers - wyswietla liste dostepnych serwerow\n users - wyswietla liste dostepnych userow\n"
