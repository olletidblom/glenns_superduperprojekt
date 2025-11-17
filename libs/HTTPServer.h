#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  http_server_initialized = 0,
  http_server_awaiting_connection = 1,
  http_server_connected = 2,
  http_server_parse_header = 3,
  http_server_POST = 4,
  http_server_dispose = 5,
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
  TCPServer *tcp_server;

  int response_code;

  char* method_url;
	char* host;
	char* url_path;
	char* url;

} HTTPServer;

int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server);
HTTPServer *HTTP_run(HTTP_Method method, void (*_Callback)());

void HTTP_work(void *_Context, uint64_t monTime);

#endif
