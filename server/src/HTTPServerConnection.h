#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "Handlers/weather_handler.h"
#include "../../libs/smw.h"
#include <stddef.h>
#include <stdint.h>

typedef HTTPServerConnection HTTPServerConnection;

/*
typedef struct
{
    char* message;
}HTTP_Response;
*/
typedef enum {
  HTTPServerConnection_State_Read_Make_URL,
  HTTPServerConnection_State_Handlers,
  HTTPServerConnection_State_Response,
  HTTPServerConnection_State_Cleanup,
} HTTPServerConnection_State;


struct HTTPServerConnection {
  smw_task *task;

  void *context;

  int socket;

  HTTPServerConnection_State state;

  char* method_url;
	char* host;
	char* url_path;
	char* url;

    Route* routes;
    size_t route_count;
    size_t max_routes;

    char recv_buffer[2048];
    size_t recv_buffer_length;
    int content_length;
    
    char* request_body;  // POST/PUT body data

    void* handler;       // Function pointer (void* to avoid circular dependency)
    char* response_body;
    int status_code;

};

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, void* server_context);

int HTTPServerConnection_ParseHeader(HTTPServerConnection *connection);
int HTTPServerConnection_BuildURL(HTTPServerConnection *connection);
int HTTPServerConnection_SendResponse(HTTPServerConnection* connection, char *body);

void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
