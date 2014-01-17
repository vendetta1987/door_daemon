#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "app_types.h"

#ifdef DAEMON

#include <syslog.h>

#define DEF_LOG_LEVEL LOG_NOTICE

#endif

#ifdef RPI

#define SLEEP 300
#define DEFAULT_PIN 0

#include <wiringPi.h>

#else

#define SLEEP 50000
#define POSIX_STDIN 0

#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>

#endif

//Read in voltage level
bool_t getCircuitState();

//Initalize IO operations
bool_t initIO();

//Set pin to reading mode
bool_t setPinReadMode(int pin);

//Set pin to write mode
bool_t setPinWriteMode(int pin);
