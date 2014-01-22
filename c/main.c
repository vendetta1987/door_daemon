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
