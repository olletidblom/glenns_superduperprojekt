#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "../../libs/smw.h"
#include "../../libs/HTTP/HTTPClient.h"
#include "../../libs/HTTP/HTTPParser.h"
#include <stddef.h>
#include <stdint.h>

//Function pointer for handlers
typedef char *(*RouteFunction)(void *_Context);

typedef RouteFunction (*OnParse)(void *_Context, char *path);


typedef enum
{
  HTTPServerConnection_State_Read, //Reads data from socket
  HTTPServerConnection_State_Parse, //Parse HTTP header
  HTTPServerConnection_State_FindRoute, //Finds route based on endpoint, returns function pointer
  HTTPServerConnection_State_Handlers, //Calls function pointer returned by FindRoute
  HTTPServerConnection_State_Response, //Sends respose to client
  HTTPServerConnection_State_Cleanup, //Disposes the connection, frees memory
} HTTPServerConnection_State;

typedef struct{
  int status_code;
  char* response_body;
}HTTPResponse;

typedef struct
{

  HTTPClient_s http_client;

  smw_task *task;

  void *context;

  int socket;

  HTTPServerConnection_State state;

  HTTPRequest http_request; // Define in HTTPParser.h
  HTTPResponse http_response;

  int *is_active;

  OnParse handler_parse;
  RouteFunction handler_process; 
} HTTPServerConnection;

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, int *is_active);

void HTTPServerConnection_SetCallback(void *_Connection, void *_Context, OnParse onHandle);

//This function is called internally, HTTPServerConnection disposes itself
void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
