#ifndef HTTP_H
#define HTTP_H

#define _POSIX_C_SOURCE 200809L

#include "TCPClient.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  http_client_initialized,
  http_client_connected,
  http_client_POST,
  http_client_header,
  http_client_GET,
  http_client_parse_header,
  http_client_dispose,
} HTTP_Status;

typedef enum {
  http_failed_to_connect = 1,
  http_failed_to_read = 2

} HTTP_Error;

typedef enum {
  http_GET = 1,
  http_POST = 2,
} HTTP_Method;

/*
typedef struct
{
    char* message;
}HTTP_Response;
*/

typedef struct {
  smw_task *task;
  void (*callback)();
  void *context;
  HTTP_Status status;
  HTTP_Method method;
  TCPClient *tcp_client;
  char *HTTP_response;
  char *url;
  int response_code;

} HTTPClient;

int HTTP_Initialize(HTTP_Method method, HTTPClient **client);

HTTPClient *HTTP_run(HTTP_Method method, void (*_Callback)());

void HTTP_work(void *_Context, uint64_t monTime);

#endif
