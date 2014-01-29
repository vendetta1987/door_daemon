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

#ifndef PIN_POLLING_H
#define PIN_POLLING_H

#include "app_types.h"

#ifdef RPI

#define SLEEP 1000
#define DEFAULT_PIN 0

//Initalize IO operations
bool_t initIO();

//Set pin to reading mode
bool_t setPinReadMode(int pin);

//Set pin to write mode
bool_t setPinWriteMode(int pin);

#else

#define SLEEP 50000
#define POSIX_STDIN 0

#endif

//Read in voltage level
bool_t getCircuitState();

#endif //PIN_POLLING_H
