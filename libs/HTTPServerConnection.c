#include "HTTPServerConnection.h"
#include "CURL.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int HTTPServerConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length);

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Disconnect(HTTPServerConnection *connection);

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, Request _HandleRequest, ProcessRequest _ProcessRequest, SendRequest _SendRequest)
{
  if (connection == NULL)
    return -1;

  HTTPServerConnection *_Connection = (HTTPServerConnection *)malloc(sizeof(HTTPServerConnection));
  _Connection->buffer_capacity = 8192;
  _Connection->buffer_length = 0;
  _Connection->buffer = (uint8_t *)malloc(_Connection->buffer_capacity);

  _Connection->socket = socket;

  _Connection->handle_request = _HandleRequest;
  _Connection->process_request = _ProcessRequest;
  _Connection->send_request = _SendRequest;

  _Connection->state = state_initialized;
  _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);

  *connection = _Connection;
  return 0;
}

int HTTPServerConnection_SendResponse(HTTPServerConnection *connection)
{
  if (connection == NULL)
    return -1;

  if(connection->response == NULL)
  return -2;


  size_t header = 1024;
  size_t total_size = header + connection->response_size + 1;

  char *response = (char *)malloc(total_size);

  int length = snprintf(response, total_size, "HTTP/1.1 200 OK\r\n"
                                              "Content-Length: %zu\r\n"
                                              "Content-Type: application/json\r\n"
                                              "\r\n"
                                              "%s",
                        connection->response_size, connection->response);
  int result = tcpserver_send(connection->socket, response, length);
  free(response);
  printf("hellooooo\n");
  return result;
}


void HTTPServerConnection_Dispose(HTTPServerConnection **connection);

void HTTPServerConnection_work(void *_Context, uint64_t monTime)
{
  HTTPServerConnection *connection = (HTTPServerConnection *)_Context;

  if (connection == NULL)
    return;

  switch (connection->state)
  {
  case state_initialized:
    ssize_t bytesRead = tcpserver_recieve(connection->socket, connection->buffer + connection->buffer_length, connection->buffer_capacity - connection->buffer_length - 1);

    if (bytesRead <= 0)
      break;

    connection->buffer_length += bytesRead;

    connection->state = state_parsing;
    break;
  case state_parsing:
    if (connection->handle_request(connection) != 0)
    {
      connection->state = state_dispose;
      break;
    }
    else
    {
      connection->state = state_processing;
      break;
    }
  case state_processing:
    if (connection->process_request(connection) != 0)
    {
      connection->state = state_dispose;
      break;
    }
    else
    {
      connection->state = state_send_request;
      break;
    }
  case state_send_request:
    if (connection->send_request(connection) != 0)
    {
      connection->state = state_dispose;
      break;
    }
    else
    {
      connection->state = state_send_response;
      break;
    }
  case state_send_response:
    HTTPServerConnection_SendResponse(connection);
    connection->state = state_dispose;
    break;
  case state_dispose:
    HTTPServerConnection_Dispose(&connection);
    return;
  default:
    break;
  }

  return;
}


void HTTPServerConnection_Dispose(HTTPServerConnection **connection)
{
  if (connection == NULL || *connection == NULL)
    return;

  HTTPServerConnection *_server = *connection;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);

  close(_server->socket);
  free(_server->buffer);
  free(_server->request);
  free(_server->response);
  free(_server->url);
  free(_server);
  *connection = NULL;
}
