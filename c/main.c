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

//Compiling commands
//For normal use:
//gcc main.c pin_polling.c data_socket.c -pthread -DDEBUG -std=gnu99 -Wall -g -o NAME
//For use as daemon:
//gcc main.c pin_polling.c data_socket.c -pthread -DDEBUG -DDAEMON -std=gnu99 -Wall -g -o NAME
//For normal use on Raspberry Pi
//gcc main.c pin_polling.c data_socket.c -pthread -lwiringPi -DDEBUG -DRPI -std=gnu99 -Wall -g -o NAME
//For daemon use on Raspberry Pi
//gcc main.c pin_polling.c data_socket.c -pthread -lwiringPi -DDEBUG -DRPI -DDAEMON -std=gnu99 -Wall -g -o NAME

#include "pin_polling.h"
#include "app_utils.h"

int main(int argc, char** argv){

    gLogLevel = DBG;

#ifdef DEBUG
    START_LOGGING("door_daemon", LOG_PID, LOG_LOCAL0);
#endif

    debug(DBG, "%s\n", "Starting Door Daemon C program component...");

    initIO();
    setPinReadMode(0);
    int ret = getCircuitState();

    debug(DBG, "%s\n", "Stopping Door Daemon C program component...");

#ifdef DEBUG
    STOP_LOGGING();
#endif

    return  ret ? EXIT_SUCCESS : EXIT_FAILURE;
}
