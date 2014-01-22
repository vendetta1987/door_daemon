#include "app_types.h"

#ifdef RPI

#define SLEEP 300
#define DEFAULT_PIN 0

#include <wiringPi.h>

#else

#define SLEEP 50000
#define POSIX_STDIN 0

#endif

//Read in voltage level
bool_t getCircuitState();

//Initalize IO operations
bool_t initIO();

//Set pin to reading mode
bool_t setPinReadMode(int pin);

//Set pin to write mode
bool_t setPinWriteMode(int pin);
