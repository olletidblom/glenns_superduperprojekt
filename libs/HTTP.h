#ifndef HTTP_H
#define HTTP_H

#include "smw.h"
#include "TCPClient.h"
#include <stddef.h>

typedef enum {
    http_client_initialized = 0,
    http_client_connected = 1,
    http_client_POST = 2, 
    http_client_header = 4,
    http_client_GET = 5,
    http_client_parse_header = 6,
    http_client_dispose = 7,
} HTTP_Status;

typedef enum{
    http_failed_to_connect = 1,
    http_failed_to_read = 2

}HTTP_Error;

typedef enum{
    http_GET = 1,
    http_POST = 2,
}HTTP_Method;

/*
typedef struct
{
    char* message;
}HTTP_Response;
*/

typedef struct 
{
    smw_task* task;
    void (*callback)();
    void* context;
    HTTP_Status status;
    HTTP_Method method;
    TCPClient* tcp_client;
    char* HTTP_response;
    char* url;
    int response_code;

}HTTPClient;






HTTPClient* HTTP_run(HTTP_Method method, void (*_Callback)());

int HTTP_work(void* _Context);

#endif