#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "app_types.h"

#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 20000

//List of available mutexes for use in main and socket thread
pthread_mutex_t g_messageMutex;
pthread_mutex_t g_messageReadyFlagMutex;
//Message struct which is used to exchange messages between threads
sock_message_t g_ClientMessage;
//Flag which denotes if a new message is ready to be sent
//bool_t g_NewMessageReady = FALSE;

bool_t createConnection();
bool_t sendMessage();
bool_t closeConnection();

#endif //TCP_SOCKET_H
