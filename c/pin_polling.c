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

#include "pin_polling.h"
#include "data_socket.h"
#include "app_utils.h"
#include "app_includes.h"

#include <unistd.h>
#include <time.h>

#ifdef RPI
#include <wiringPi.h>
#else
#include <poll.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#define CON_RETRIES_COM_THREAD 5
#define CON_RETRIES_WAIT_PERIOD 0xf4240 //1000000

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
static void getParsableTime(doorState_t state, char **pTime);
static void triggerEventMessage(doorState_t *state);
static void handlePinInterrupt(void);

//door state variable for callback
static doorState_t isr_doorState;

bool_t getCircuitState(){

#if defined(DAEMON) && !defined(RPI)
    debug(FTL, "%s\n", "Programm can not run as daemon on normal desktop Linux!");
    return FALSE;
#endif

    //Start socket thread
    createConnection();

    /*
     * This is done to ensure
     * that the socket thread is running
     * before messages are passed to the thread.
     * If the socket could not be established,
     * it will poll CON_RETRIES_COM_THREAD times/seconds
     * before returning.
     */
    int retries = 0;

    while( !socketThreadRunning() ){

        if( retries < CON_RETRIES_COM_THREAD ){
            debug(FTL, "Communication thread is not running, waiting %d more seconds...\n",
                  (CON_RETRIES_COM_THREAD - (retries)));

            usleep(CON_RETRIES_WAIT_PERIOD);
        }else if(retries == CON_RETRIES_COM_THREAD){
            debug(FTL, "%s\n", "Communication thread is not responding...abort!");
            return FALSE;
        }

        ++retries;
    }

    //door state variable for active polling
    doorState_t door = OPEN;

#ifdef RPI //Running on Raspberry Pi
#ifdef ACTIVE_POLL //Request pin state by activley polling it
    debug(DBG, "%s\n", "Starting hardware polling loop...");
    while(socketThreadRunning()){

        int level = digitalRead(DEFAULT_PIN);

        if( level == 1 && door == OPEN){

            door = CLOSED;
            triggerEventMessage(&door);

        }else if( level == 0 && door == CLOSED){

            door = OPEN;
            triggerEventMessage(&door);
        }

        delay(SLEEP);
    }

    return TRUE;
#else //Setting up an Interrupt to be notified when state changes
    debug(DBG, "%s\n", "Starting interrupt service routine...");

    //Setup up callback, this is basically a wrapper for triggering
    //triggerEventMessage() asynchronously
    if( wiringPiISR(DEFAULT_PIN, INT_EDGE_BOTH, &handlePinInterrupt) < 0 ){
        debug(FTL, "%s\n", "Error while setting up interrupt routine for pin handling!");
        return FALSE;
    }

    //Read state once to have a start value, then enter sleep loop
    isr_doorState = digitalRead(DEFAULT_PIN) ? CLOSED : OPEN;

    //Put to sleep until socket thread ends
    while(socketThreadRunning()){
        delay(SLEEP);
    }

    return TRUE;
#endif
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

    while(!end && socketThreadRunning()){

        struct pollfd pfd[1];
        int ret = 0;
        char c  = 0;

        pfd[0].fd = POSIX_STDIN;
        pfd[0].events = POLLIN;

        ret = poll(pfd, 1, 1000);

        if(ret > 0 && door == OPEN){

            door = CLOSED;
            triggerEventMessage(&door);
            read(POSIX_STDIN, &c, 1);

        }else if(ret == 0 && door == CLOSED){

            door = OPEN;
            triggerEventMessage(&door);

        }else if(ret == -1){

            if( tcsetattr(POSIX_STDIN, TCSANOW, &old_termio) != 0){

                goto ERROR;
            }

            goto ERROR;
        }

        usleep(SLEEP);
    }

    if( tcsetattr(POSIX_STDIN, TCSANOW, &old_termio) != 0)
        goto ERROR;

    return TRUE;

ERROR:
    debug(FTL, "%s\n", "Something fatal happened :(");
    return FALSE;
#endif
}

#ifdef RPI
bool_t initIO(){
    wiringPiSetup();
    return TRUE;
}

bool_t setPinReadMode(int pin){
    pinMode(pin, INPUT);
    return TRUE;
}

bool_t setPinWriteMode(int pin){
    pinMode(pin, OUTPUT);
    return TRUE;
}
#else
void sighandler(int signo){
    end = 1;
}
#endif

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

static void triggerEventMessage(doorState_t *state){

    char *pTime = 0;

    getParsableTime(*state, &pTime);
    sendMessage(pTime);
    free(pTime);
}

static void handlePinInterrupt(void){

    isr_doorState = isr_doorState == OPEN ? CLOSED : OPEN;
    triggerEventMessage(&isr_doorState);
}
