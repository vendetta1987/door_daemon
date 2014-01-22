#include "tcp_socket.h"
#include "app_utils.h"
#include "app_includes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


//Flag which denotes if a new message is ready to be sent
bool_t g_NewMessageReady = FALSE;

//Flag which denotes if the thread shall continue
static bool_t endThread = FALSE;
static bool_t g_connectionClosed = TRUE;

static pthread_mutex_t endThreadFlagMutex;

/*Local function call prototypes*/
static void* socketWorker(void* threadID);
static bool_t createUNSocketConnection(int *sockFd);

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

    bool_t end = FALSE;
    bool_t messageReady = FALSE;
    sock_message_t thread_msg;
    long tid = (long)threadID;

//#ifdef DEBUG
//#   ifdef DAEMON
//    //syslog here
//#   else
//    printf("Thread with ID %ld started!\n", tid);
//#   endif
//#endif

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
//#ifdef DEBUG
//#   ifdef DAEMON
//    //syslog here
//#   else
//            printf("Thread received new message: %s\n", thread_msg.message);
//#   endif
//#endif
            debug(DBG, "Thread received new message: %s\n", thread_msg.message);

        }

        pthread_mutex_lock(&endThreadFlagMutex);
        end = endThread;
        pthread_mutex_unlock(&endThreadFlagMutex);
    }

//#ifdef DEBUG
//#   ifdef DAEMON
//    //syslog here
//#   else
//    printf("Thread with ID %ld shutting down!\n", tid);
//#   endif
//#endif

    debug(DBG, "Thread with ID %ld shutting down!\n", tid);

    pthread_exit(NULL);
}

static bool_t createUNSocketConnection(int *sockFd){

    struct sockaddr_un  address;
    int                 size;
    char*               pBuff = malloc(1024);

    if( (*sockFd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 1){
//#ifdef DAEMON
//		//syslog here
//#else
//        perror("Error while initializing socket for UNIX Domain Socket connection: ");
//#endif
        debug(FTL, "%s", "Error while initializing socket for UNIX Domain Socket connection!");
		return FALSE;
	}

	//Open here
		
	return TRUE;
}


