#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>


/*
typedef struct
{
    char* message;
}HTTP_Response;
*/



typedef struct {
  smw_task *task;

  void *context;

  int socket;

  
  char* method_url;
	char* host;
	char* url_path;
	char* url;
} HTTPServerConnection;

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket);



void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
