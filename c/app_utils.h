#ifndef UTILS_H
#define UTILS_H

#include "app_includes.h"

#include <syslog.h>

//! Possible debug level
typedef enum {
    OFF     = 0x00, //! No log messages
    FTL     = 0x01, //! Only fatal messages which terminate the application
    ERR     = 0x02, //! Errors and fatals will be displayed
    WRN     = 0x04, //! Warnings will be displayed
    DBG     = 0x08, //! Debug messages will be displayed
    TRC     = 0x10, //! More thorough debug messages will be displayed
    INF     = 0x20, //! Trivial outputs
    ALL     = 0x40  //! Everything will be displayed
}eLogLevel;

typedef eLogLevel loglevel_t;

#define DEFAULT_LOG_LEVEL DBG

#define START_LOGGING(ident, option, facility) openlog(ident, option, facility)
#define STOP_LOGGING() closelog()

//! Conditional debug printer
#ifndef DAEMON
#define debug(lvl, fmt, ...) \
    do{ if(DEBUG && lvl <= (gLogLevel ? \
    gLogLevel : DEFAULT_LOG_LEVEL)) \
    fprintf(stderr, "%s[%d]:%s() >> " fmt, __FILE__, \
    __LINE__, __func__, __VA_ARGS__);} while(0)
#else
#define debug(lvl, fmt, ...) \
    do{ if(DEBUG && lvl <= (gLogLevel ? gLogLevel : DEFAULT_LOG_LEVEL)) \
        switch(lvl){\
        case FTL:\
            syslog(LOG_ALERT, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        case ERR:\
            syslog(LOG_CRIT, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        case WRN:\
            syslog(LOG_ERR, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        case DBG:\
            syslog(LOG_WARNING, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        case TRC:\
            syslog(LOG_NOTICE, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        case INF:\
            syslog(LOG_INFO, "%s[%d]:%s() >> " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);\
        break;\
        }\
        } while(0)
#endif

loglevel_t gLogLevel;

#endif // UTILS_H
