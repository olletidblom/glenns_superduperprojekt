#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L
#include "TCPServer.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>


typedef struct HTTPServerConnection HTTPServerConnection;

/*
typedef struct
{
    char* message;
}HTTP_Response;
*/

typedef enum {
  state_initialized = 0,
  state_parsing = 1,
  state_processing = 2,
  state_send_request = 3,
  state_send_response = 4,
  state_dispose = 5
} HTTPState;

typedef int (*Request)(void* _Context);
typedef int (*ProcessRequest)(void* _Context);
typedef int (*SendRequest)(void* _Context);

struct HTTPServerConnection{
  smw_task *task;

  Request handle_request;
  ProcessRequest process_request;
  SendRequest send_request;


  HTTPState state;

  int socket;

  uint8_t* buffer;
  size_t buffer_length;
  size_t buffer_capacity;

  char* response;
  size_t response_size;
  char* request;
	char* url;
};





int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, Request _HandleRequest, ProcessRequest _ProcessRequest, SendRequest _SendRequest);



void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
