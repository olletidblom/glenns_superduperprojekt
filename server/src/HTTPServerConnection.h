#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "../../libs/smw.h"
#include "../../libs/HTTPClient.h"
#include <stddef.h>
#include <stdint.h>

typedef char* (*RouteFunction)(void* _Context);

typedef RouteFunction* (*OnParse)(void* _Context, char* path);

typedef char* (*OnProcess)(void* _Context);



/*
typedef struct
{
    char* message;
}HTTP_Response;
*/
typedef enum
{
  HTTPServerConnection_State_Read_Make_URL,
  HTTPServerConnection_State_Handlers,
  HTTPServerConnection_State_Response,
  HTTPServerConnection_State_Cleanup,
} HTTPServerConnection_State;

typedef struct
{

  HTTPClient_s http_client;

  smw_task *task;

  void *context;

  int socket;

  HTTPServerConnection_State state;

  char *url;

  char recv_buffer[2048];
  size_t recv_buffer_length;
  int content_length;
  int* is_active;
  char *request_body; // POST/PUT body data

  OnParse handler_parse;
  OnProcess handler_process;  // Function pointer (void* to avoid circular dependency)
  char *response_body;
  int status_code;
}HTTPServerConnection;

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, int* is_active);

int HTTPServerConnection_ParseHeader(HTTPServerConnection *connection);

int HTTPServerConnection_SendResponse(HTTPServerConnection *connection, char *body);

void HTTPServerConnection_SetCallback(void* _Connection, void* _Context, OnParse onHandle);

void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
