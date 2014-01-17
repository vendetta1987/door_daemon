#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdlib.h>

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
