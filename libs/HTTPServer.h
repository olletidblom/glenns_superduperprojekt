#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "HTTPServerConnection.h"
#include "Request.h"
#include "API/API.h"
#include "CURL.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>





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
  HTTP_Method method;
  TCPServer *tcp_server;


  HTTPServerConnection* connection;
  void* context;
  int response_code;

  char* method_url;
	char* host;
	char* url_path;
	char* url;

} HTTPServer;

int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server);



void HTTPServer_Dispose(HTTPServer **server);
#endif
