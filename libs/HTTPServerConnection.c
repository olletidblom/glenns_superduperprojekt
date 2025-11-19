#include "HTTPServerConnection.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


int HTTPConnection_Read(HTTPServerConnection *connection, uint8_t *buffer, size_t length)
{
  int result = recv(connection->socket, buffer, length, MSG_DONTWAIT);
  // printf("send error %d: %s\n", errno, strerror(errno));
  return result;
}

int HTTPConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length)
{
  ssize_t result = send(connection->socket, buffer, length, MSG_NOSIGNAL);
  // printf("send error %d: %s\n", errno, strerror(errno));
  // scanf("%d", result);
  return result;
}

int HTTPServerConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length);

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Disconnect(HTTPServerConnection *connection);

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket)
{
  if (connection == NULL)
    return -1;


  HTTPServerConnection *_Connection = (HTTPServerConnection *)malloc(sizeof(HTTPServerConnection));

  _Connection->socket = socket;

  _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);


  *connection = _Connection;
  return 0;
}


int HTTPServerConnection_SendResponse(HTTPServerConnection* connection, char *body)
{
  if (connection == NULL)
    return -1;

  body = "{\"name\":\"Alice\",\"age\":30}";


  char response[1024];
  int length = snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n" "Content-Length: %zu\r\n" "Content-Type: application/json\r\n" "\r\n" "%s", strlen(body), body);
  int result = HTTPConnection_Write(connection, response, length);
  tcpserver_disconnect(connection->socket);
  return result;
}


void HTTPServerConnection_work(void *_Context, uint64_t monTime)
{
  HTTPServerConnection *connection = (HTTPServerConnection *)_Context;

  if (connection == NULL)
    return;


  connection->method_url = NULL;
  connection->url = NULL;
  connection->host = NULL;
  connection->url_path = NULL;

  char buffer[1024];

  int bytesRead = HTTPConnection_Read(connection, (uint8_t *)buffer, sizeof(buffer));

  if (bytesRead < 0)
  {
    return;
  }

  if (bytesRead > 0)
  {
    // printf("Data: %s\n", buffer);
    buffer[bytesRead] = '\0';
    char *ptr = &buffer[0];
    char *method = strchr(ptr, ' ');
    if (method != NULL)
    {
      int method_length = method - ptr;

      connection->method_url = strndup(ptr, method_length);

      if (connection->method_url == NULL)
      {
        printf("HTTPServerConnection_TaskWork: Failed to copy method\n");
        return;
      }
    }

    char *url_path = strchr(ptr, '/');

    if (url_path != NULL)
    {
      char *url_end = strchr(url_path, ' ');

      if (url_end != NULL)
      {
        int url_length = url_end - url_path;

        connection->url_path = strndup(url_path, url_length);
        if (connection->url_path == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
          return;
        }
      }
    }

    char *host = strstr(ptr, "Host: ");
    if (host != NULL)
    {
      host += strlen("Host: ");

      char *eol = strstr(host, "\r\n");

      if (eol != NULL)
      {
        int url_length = eol - host;

        connection->host = strndup(host, url_length);

        if (connection->host == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
          return;
        }
      }
    }
  }

  if (connection->method_url != NULL && connection->host != NULL && ((strcmp(connection->method_url, "POST") == 0) || (strcmp(connection->method_url, "GET") == 0)))
  {
    char temp_buffer[1024];

    snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", connection->host, connection->url_path);
    // printf("Temp URL: %s\n", temp_buffer);
    free(connection->url_path);
    free(connection->host);
    free(connection->method_url);

    connection->url = strdup(temp_buffer);
    if (strstr(buffer, "\r\n\r\n")){
      printf("HTTP connection: Finished parsing header: %s SOCKET: %d\n", connection->url, connection->socket);
      HTTPServerConnection_SendResponse(connection, NULL);
    }

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
  free(_server);
  *connection = NULL;
}
