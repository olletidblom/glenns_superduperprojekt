#ifndef HTTP_SERVER_CONNECTION_H
#define HTTP_SERVER_CONNECTION_H

#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
#include <stdint.h>

// Function pointer for handlers
typedef char *(*RouteFunction)(void *_Context);

typedef void (*Callback)(void *_Context);

typedef struct
{
  char *key;
  char *value;
} InputParameters;

typedef struct
{
  char *end_point;
  int pairsLength;
  int maxPairs;
  InputParameters *parameters;
} URLHandler;

typedef struct
{
  char *url;
  size_t url_length;
  char recv_buffer[2048];
  size_t recv_buffer_length;

  int content_length;

  char *request_body;
} HTTPRequest;

typedef struct
{
  int status_code;
  char *response_ptr;
  char *response;
  char *response_formatted;
  int response_length;
} HTTPResponse;

typedef enum
{
  HTTPServerConnection_State_Read,           // Reads data from socket
  HTTPServerConnection_State_Parse,          // Parse HTTP header
  HTTPServerConnection_State_ParseURL,       // Parse URL to get endpoint and input parameters
  HTTPServerConnection_State_FindRoute,      // Finds route based on endpoint, returns function pointer
  HTTPServerConnection_State_Handlers,       // Calls function pointer returned by FindRoute
  HTTPServerConnection_State_FormatResponse, // Formats response
  HTTPServerConnection_State_SendResponse,   // Sends respose to client
  HTTPServerConnection_State_Timeout,
  HTTPServerConnection_State_Cleanup, // Disposes the connection, frees memory
} HTTPServerConnection_State;

typedef enum
{
  HTTPServerConnection_ReadResult_Success,
  HTTPServerConnection_ReadResult_Pending,
  HTTPServerConnection_ReadResult_Error,

} HTTPServerConnection_ReadResult;

typedef struct
{

  URLHandler *url_handler;

  void *context;

  int socket;

  HTTPServerConnection_State state;

  HTTPRequest http_request;
  HTTPResponse http_response;

  uint64_t timeout;

  int bytesReadOut;
  int *is_active;
  Callback connection_callback;
  RouteFunction handler;
} HTTPServerConnection;

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, int *is_active);

int URLHandler_Initialize(URLHandler **_URLHandler);

int HTTPRequest_ParseHeader(HTTPRequest *http_request);

int HTTPRequest_Initialize(HTTPRequest *http_request);

int URLHandler_Parse(URLHandler *_URLHandler, char *url);

void HTTPServerConnection_SetCallback(void *_Connection, void *_Context, Callback _Callback);

void URLHandler_Dispose(URLHandler **_URLHandler);

// This function is called internally, HTTPServerConnection disposes itself
void HTTPServerConnection_Dispose(HTTPServerConnection **server);
#endif
