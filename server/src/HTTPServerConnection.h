#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "../../libs/smw.h"
#include <stddef.h>
#include <stdint.h>


/*
typedef struct
{
    char* message;
}HTTP_Response;
*/
typedef enum {
  HTTPServerConnection_State_Read_header,
  HTTPServerConnection_State_Route,
  HTTPServerConnection_State_Handlers,
  HTTPServerConnection_State_Response,
  HTTPServerConnection_State_Cleanup,
} HTTPServerConnection_State;


typedef struct {
  smw_task *task;

  void *context;

  int socket;

  HTTPServerConnection_State state;

  char* method_url;
	char* host;
	char* url_path;
	char* url;

    char recv_buffer[4096];
    size_t recv_buffer_length;
    int content_length;

} HTTPServerConnection;

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, void* server_context);



void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
