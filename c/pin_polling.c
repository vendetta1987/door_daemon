#include "pin_polling.h"
#include "tcp_socket.h"

extern bool_t g_NewMessageReady;

/*Door state type*/
typedef enum{
    OPEN 			= 0,
    CLOSED 			= 1
} eDoorStates;

typedef eDoorStates doorState_t;

#ifndef RPI
/*Globals*/
static sig_atomic_t end = 0;

/*Desktop Linux Prototypes*/
static void sighandler(int signo);
#endif

/*General Prototypes*/
static void printDoorState(doorState_t *state);
static void getParsableTime(doorState_t state, char **pTime);

bool_t getCircuitState(){

#ifdef DAEMON
    //Here Openlog
#endif

    createConnection();

    doorState_t door = OPEN;
    sock_message_t msg;
    char* pTime = 0;

#ifdef RPI //Running on Raspberry Pi
    while(1){

        int level = digitalRead(DEFAULT_PIN);

# ifdef DEBUG
#  ifndef DAEMON
        //printDoorState(&door);
        //printf("level: %d\n", level);
#  else
#  endif
# endif
        if( level == 1 && door == OPEN){

            door = CLOSED;
            getParsableTime(door, &pTime);
            strcpy(msg.message, pTime);
            msg.msg_size = strlen(msg.message);

            pthread_mutex_lock(&g_messageMutex);
            memcpy(&g_ClientMessage, &msg, sizeof(sock_message_t));
            pthread_mutex_unlock(&g_messageMutex);

            pthread_mutex_lock(&g_messageReadyFlagMutex);
            g_NewMessageReady = TRUE;
            pthread_mutex_unlock(&g_messageReadyFlagMutex);
# ifdef DAEMON
# else
            printf("JSON: %s\n", msg.message);
# endif
            free(pTime);

        }else if( level == 0 && door == CLOSED){

            door = OPEN;
            getParsableTime(door, &pTime);
            strcpy(msg.message, pTime);
            msg.msg_size = strlen(msg.message);

            pthread_mutex_lock(&g_messageMutex);
            memcpy(&g_ClientMessage, &msg, sizeof(sock_message_t));
            pthread_mutex_unlock(&g_messageMutex);

            pthread_mutex_lock(&g_messageReadyFlagMutex);
            g_NewMessageReady = TRUE;
            pthread_mutex_unlock(&g_messageReadyFlagMutex);
# ifdef DAEMON
# else
           printf("JSON: %s\n", msg.message);
# endif
           free(pTime);
        }

        delay(SLEEP);
    }
#ifdef DAEMON
    //Here closelog
#endif
    return TRUE;
#else //Running on normal Linux
    struct termios old_termio;
    struct termios new_termio;

    struct sigaction action;

    if( tcgetattr(POSIX_STDIN, &old_termio) != 0)
        goto ERROR;

    memset(&action, 0, sizeof(struct sigaction));

    action.sa_handler = sighandler;

    if( sigaction(SIGINT, &action, NULL) != 0)
        goto ERROR;

    if( sigaction(SIGQUIT, &action, NULL) != 0)
        goto ERROR;

    if( sigaction(SIGTERM, &action, NULL) != 0)
        goto ERROR;

    memset(&action, 0, sizeof(struct sigaction));

    action.sa_handler = SIG_IGN;

    if( sigaction(SIGTTOU, &action, NULL) != 0)
        goto ERROR;

    if( tcgetattr(POSIX_STDIN, &new_termio) != 0)
        goto ERROR;

    new_termio.c_lflag &= ~(ICANON | ECHO);

    if( tcsetattr(POSIX_STDIN, TCSANOW, &new_termio) != 0)
        goto ERROR;

    while(!end){

        struct pollfd pfd[1];
        int ret = 0;
        char c  = 0;

        pfd[0].fd = POSIX_STDIN;
        pfd[0].events = POLLIN;

        ret = poll(pfd, 1, 1000);

# ifdef DEBUG
#  ifndef DAEMON
        printDoorState(&door);
        printf("Ret: %d\n", ret);
#  else
#  endif
# endif

        if(ret > 0 && door == OPEN){

            door = CLOSED;
# ifndef DAEMON
            printf("Door open!\n");
# else
            //Syslog here
# endif
            read(POSIX_STDIN, &c, 1);

        }else if(ret == 0 && door == CLOSED){

            door = OPEN;
# ifndef DAEMON
            printf("Door closed!\n");
# else
            //Syslog here
# endif

        }else if(ret == -1){

            if( tcsetattr(POSIX_STDIN, TCSANOW, &old_termio) != 0){

                goto ERROR;
            }

            goto ERROR;
        }

        //usleep(SLEEP);
    }

    if( tcsetattr(POSIX_STDIN, TCSANOW, &old_termio) != 0)
        goto ERROR;

    return TRUE;

ERROR:
# ifndef DAEMON
    perror("pin_polling.c");
# else
    //Syslog here
    //here closelog
# endif
    return FALSE;
#endif
}

bool_t initIO(){
#ifdef RPI
    wiringPiSetup();
#endif
    return TRUE;
}

bool_t setPinReadMode(int pin){
#ifdef RPI
    pinMode(pin, INPUT);
#endif
    return TRUE;
}

bool_t setPinWriteMode(int pin){
#ifdef RPI
    pinMode(pin, OUTPUT);
#endif
    return TRUE;
}

#ifndef RPI

void sighandler(int signo){
    end = 1;
}

#endif

void printDoorState(doorState_t* state){
    switch(*state){
    case OPEN:
        printf("State: Open\n");
        break;
    case CLOSED:
        printf("State: Closed\n");
        break;
    }
}

void getParsableTime(doorState_t state, char** pTime){

    struct tm *pTimeInfo;
    time_t raw;

    raw = time(NULL);
    pTimeInfo = localtime(&raw);

    *pTime = malloc(sizeof(char) * JSON_STRING_LENGTH);

    /* Generating a JSON formatted String
     * for easy parsing in Python script
     * {"State": "XX", "Time": "XX:XX:XX", "Date": "XX-XX-XXXX"}
     */

    switch(state){
    case OPEN:
        strftime(*pTime,
             JSON_STRING_LENGTH,
             "{\"State\": \"DO\", \"Time\": \"%H:%M:%S\", \"Year\": \"%d-%m-%Y\"}",
             pTimeInfo);
    break;
    case CLOSED:
         strftime(*pTime,
             JSON_STRING_LENGTH,
             "{\"State\": \"DC\", \"Time\": \"%H:%M:%S\", \"Year\": \"%d-%m-%Y\"}",
             pTimeInfo);
    break;
    }
}
