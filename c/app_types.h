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

#ifndef APP_TYPES_H
#define APP_TYPES_H

#include "app_includes.h"

#define JSON_STRING_LENGTH 60

//Boolean enum substitute
typedef enum{
    FALSE = 0,
    TRUE = 1
}eBOOL_t;

typedef eBOOL_t bool_t;

//Message to be sent via socket
//Passed to socket thread later
typedef struct{
    char message[JSON_STRING_LENGTH];
    size_t msg_size;
}stMSG_t;

typedef stMSG_t sock_message_t;

#endif // APP_TYPES_H
