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

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "app_types.h"

#include <pthread.h>

#define COM_SOCKET "/tmp/door_daemon_data_socket"

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
