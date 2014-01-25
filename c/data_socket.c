/*
 * Copyright (c) 2014 Michael Flau <michael@flau.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "data_socket.h"
#include "app_utils.h"
#include "app_includes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


//Flag which denotes if a new message is ready to be sent
static bool_t g_NewMessageReady = FALSE;

//Flag which denotes if the thread shall continue
static bool_t endThread = FALSE;
static bool_t g_connectionClosed = TRUE;

static pthread_mutex_t endThreadFlagMutex;

/*Local function call prototypes*/
static void* socketWorker(void* threadID);
static bool_t createUNSocketConnection(int *sockFd);
static bool_t transmitViaUNSocketConnection(int *sockFd, sock_message_t* msg);
static bool_t closeUNSocketConnection(int *sockFd);

/**
 * @brief createConnection
 * This will create a socket connection to a running server.
 * The socket is created in an own thread. The start functionality
 * of the thread is implemented here.
 * @return
 * TRUE(1) if creation was successful else FALSE(0)
 */
bool_t createConnection(){

    long threadID = 1;

    pthread_t socket_thread;
    pthread_attr_t attr;

    //Initialize Mutexes
    pthread_mutex_init(&g_messageMutex,          NULL);
    pthread_mutex_init(&g_messageReadyFlagMutex, NULL);
    pthread_mutex_init(&endThreadFlagMutex,      NULL);

    //Initialize thread attribute
    if( pthread_attr_init(&attr) != 0){
        perror("Thread attribute initialization failed: ");
        return FALSE;
    }

    //Set detached state
    if( pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0){
        perror("Failed to set thread attribute: ");
        return FALSE;
    }

    //Start thread
    if( pthread_create(&socket_thread, &attr, socketWorker, (void *)threadID) != 0){
        perror("Failed to create socket thread: ");
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief closeConnection
 * This function sets a global flag which will force the thread to
 * close the established socket, and to end the thread.
 * @return
 */
bool_t closeConnection(){

    pthread_mutex_lock(&endThreadFlagMutex);
    endThread = TRUE;
    pthread_mutex_unlock(&endThreadFlagMutex);

    return TRUE;
}

bool_t sendMessage(){

    pthread_mutex_lock(&g_messageReadyFlagMutex);
    g_NewMessageReady = TRUE;
    pthread_mutex_unlock(&g_messageReadyFlagMutex);

    return TRUE;
}

void* socketWorker(void *threadID){

    bool_t end          = FALSE;
    bool_t messageReady = FALSE;
    sock_message_t thread_msg;
    long tid            = (long)threadID;
    int socketFD        = 0;

    if( !createUNSocketConnection(&socketFD) ){
        debug(FTL, "%s\n", "Socket related error, ending thread!");
        end = TRUE;
    }

    debug(DBG, "Thread with ID %ld started!\n", tid);

    while(end != TRUE){

        pthread_mutex_lock(&g_messageReadyFlagMutex);
        messageReady = g_NewMessageReady;
        pthread_mutex_unlock(&g_messageReadyFlagMutex);

        if(messageReady){
            pthread_mutex_lock(&g_messageMutex);
            memcpy(&thread_msg, &g_ClientMessage, sizeof(sock_message_t));
            pthread_mutex_unlock(&g_messageMutex);

            pthread_mutex_lock(&g_messageReadyFlagMutex);
            g_NewMessageReady = FALSE;
            pthread_mutex_unlock(&g_messageReadyFlagMutex);

            if( !transmitViaUNSocketConnection(&socketFD, &thread_msg) ){
                debug(FTL, "%s\n", "Socket related error, ending thread!");
                end = TRUE;
                pthread_exit(NULL);
            }

            debug(DBG, "Thread received new message: %s\n", thread_msg.message);

        }

        pthread_mutex_lock(&endThreadFlagMutex);
        end = endThread;
        pthread_mutex_unlock(&endThreadFlagMutex);
    }

    debug(DBG, "Thread with ID %ld shutting down!\n", tid);

    if( !closeUNSocketConnection(&socketFD) ){
        debug(FTL, "%s\n", "Socket related error, ending thread!");
    }

    pthread_exit(NULL);
}

static bool_t createUNSocketConnection(int *sockFd){

    struct sockaddr_un address;

    if( (*sockFd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 1){
        debug(FTL, "%s", "Error while initializing socket for UNIX Domain Socket connection!");
		return FALSE;
	}

    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, COM_SOCKET);

    if( connect(*sockFd, (struct sockaddr*) &address, sizeof(address)) != 0 ){
        debug(FTL, "%s", "Error while connecting to server!");
        return FALSE;
    }

    debug(DBG, "%s\n", "Connection with Python string server established!");
		
	return TRUE;
}

static bool_t transmitViaUNSocketConnection(int* sockFd, sock_message_t* msg){

    if(send(*sockFd, msg->message, msg->msg_size, 0) == -1){
        debug(FTL, "%s\n", "Error while sending message to server!");
        return FALSE;
    }

    debug(DBG, "%s\n", "Message sent to server!");

    return TRUE;
}

static bool_t closeUNSocketConnection(int *sockFd){

    if( close(*sockFd) != 0 ){
        debug(FTL, "%s\n", "Error while closing socket connection!");
        return FALSE;
    }

    debug(DBG, "%s\n", "Socket to server closed!");

    return TRUE;
}
