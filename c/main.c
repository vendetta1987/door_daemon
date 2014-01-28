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

#ifdef DAEMON
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOCK_FILE "doord.lock"
#define RUNNING_DIR "/tmp/"

static void daemonize();
static void sigHandler(int event);
#endif

#include "pin_polling.h"
#include "app_utils.h"

int main(int argc, char** argv){

    gLogLevel = DBG;

#if defined(DEBUG) && defined(DAEMON)
    START_LOGGING("door_daemon", LOG_PID, LOG_LOCAL0);
#endif

    debug(DBG, "%s\n", "Starting Door Daemon C program component...");

#ifdef RPI
    initIO();
    setPinReadMode(0);
#ifdef DAEMON
    daemonize();
#endif
#endif

    int ret = getCircuitState();

    debug(DBG, "%s\n", "Stopping Door Daemon C program component...");

#if defined(DEBUG) && defined(DAEMON)
    STOP_LOGGING();
#endif

    return  ret ? EXIT_SUCCESS : EXIT_FAILURE;
}

#ifdef DAEMON
static void daemonize(){

    // Check if process instance
    // is a daemon already
    if(getppid() == 1){
        debug(DBG, "%s\n", "Already a daemon...aborting");
        return;
    }

    int pid = 0;

    switch(pid = fork()){
    case 0:         //Child
    {
        setsid(); //Get new process group

        int idx = getdtablesize();

        for(; idx >= 0; --idx)
            close(idx);

        idx = open("/dev/null", O_RDWR);//0 - stdout
        dup(idx);                       //1 - stdin
        dup(idx);                       //2 - stderr

        umask(027);

        chdir(RUNNING_DIR);

        int fd_lock = open(LOCK_FILE, O_RDWR|O_CREAT, 0640);

        //handle open() errors...

        if(fd_lock < 0)
            exit(EXIT_FAILURE);

        if( lockf(fd_lock, F_TLOCK, 0) < 0 )
            exit(EXIT_SUCCESS);

        //Ignore following signals
        signal(SIGCHLD, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);

        //Handle following signals
        signal(SIGHUP, sigHandler);
        signal(SIGTERM, sigHandler);
    }
        break;
    case -1:        //Error
        debug(FTL, "%s", "Error while daemonizing process!");
        exit(EXIT_FAILURE);
        break;
    default:        //Parent
        exit(EXIT_SUCCESS);
        break;

    }
}

static void sigHandler(int event){
    switch(event){
    case SIGHUP:
        debug(DBG, "%s\n", "Received SIGHUP signal!");
        break;
    case SIGTERM:
        debug(FTL, "%s\n", "Received SIGTERM signal!");
        //end thread here, disconnect from socket...
        exit(EXIT_SUCCESS);
        break;
    }
}
#endif
