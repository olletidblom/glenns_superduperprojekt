#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "Handlers/handler.h"
#include "HTTPServerConnection.h"
#include "../../libs/smw.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  http_server_initialized = 0,
  http_server_awaiting_connection = 1,
  http_server_connection_received = 2,
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
  HTTP_Status status;
  HTTP_Method method;
  TCPServer *tcp_server;

  Route* routes;
    size_t route_count;
    size_t max_routes;

  void* context;
  int response_code;

  char* method_url;
	char* host;
	char* url_path;
	char* url;

} HTTPServer;


RequestHandler HTTPServer_FindHandler(HTTPServer* server, const char* method, const char* path);



int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server);

void HTTPServer_RegisterRoute(HTTPServer* server, const char* method, 
                              const char* path, RequestHandler handler);

void HTTPServer_Dispose(HTTPServer **server);
#endif
