#include "HTTPServer.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int HTTP_Write(HTTPServer *server, char *buffer, size_t length);

void HTTP_Dispose(HTTPServer **server);

void strtokRepeat(char *str, int repetition)
{
  char *myPtr = strtok(str, " ");
  int i;
  for (i = 0; i < (repetition - 1); i++)
  {
    myPtr = strtok(NULL, " ");
  }
}

int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server)
{
  TCPServer *tcp_server = (TCPServer *)malloc(sizeof(TCPServer));

  if (server == NULL)
    return -1;

  HTTP_Status status;
  HTTPServer *http_server = (HTTPServer *)malloc(sizeof(HTTPServer));

  http_server->task = smw_create_task(http_server, HTTP_work);
  http_server->status = http_server_initialized;
  http_server->method = method;

  http_server->tcp_server = tcp_server;

  tcpserver_listen(http_server->tcp_server, 8080, 10);

  *server = http_server;
}

int HTTP_Accept(HTTPServer *server)
{
  int result = tcpserver_accept(server->tcp_server);

  if (result < 0)
    printf("HTTP: Failed to connect to client\n");

  return result;
}

int HTTP_Write(HTTPServer *server, char *buffer, size_t length)
{
  int result = tcpserver_send(server->tcp_server, (char *)buffer, length);

  if (result < 0)
    printf("HTTP: Failed to send data\n");

  printf("message sent: %s\n", buffer);
  return result;
}

int HTTP_Read(HTTPServer *server, uint8_t *buffer, size_t length)
{
  int result = tcpserver_recieve(server->tcp_server, buffer, length);

  if (result < 0)
    printf("HTTP: Failed to receive data\n");
  //server->HTTP_response = strndup(buffer, result);
  return result;
}

int HTTP_ParseHeader(HTTPServer *server)
{
  /*
  char *myPtr = strtok(myStr, " ");
  myPtr = strtok(NULL, " ");
  */

  printf("HTTP Server: Parsing header\n");
  if (server == NULL)
    return;

  server->method_url = NULL;
  server->url = NULL;
  server->host = NULL;
  server->url_path = NULL;

  char buffer[1024];

  int bytesRead = HTTP_Read(server, (uint8_t *)buffer, sizeof(buffer));
  printf("HTTP Server: Read %d bytes\n", bytesRead);
  if(bytesRead < 0)
  {
    return -1;
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

      server->method_url = strndup(ptr, method_length);

      if (server->method_url == NULL)
      {
        printf("HTTPServerConnection_TaskWork: Failed to copy method\n");
      }
    }

    char *url_path = strchr(ptr, '/');

    if (url_path != NULL)
    {
      char *url_end = strchr(url_path, ' ');

      if (url_end != NULL)
      {
        int url_length = url_end - url_path;

        server->url_path = strndup(url_path, url_length);
        if (server->url_path == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
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

        server->host = strndup(host, url_length);

        if (server->host == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
        }
      }
    }
  }

  if (server->method_url != NULL && server->host != NULL && (strcmp(server->method_url, "GET") == 0))
  {
    char temp_buffer[1024];

    snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", server->host, server->url_path);
    // printf("Temp URL: %s\n", temp_buffer);
    free(server->url_path);
    free(server->host);
    free(server->method_url);

    server->url = strdup(temp_buffer);
    if (strstr(buffer, "\r\n\r\n"))
    printf("HTTP Server: Finished parsing header: %s\n", server->url);
      //server->onRequest(server->context);
  }
  return 0;
}

int HTTPServer_SendResponse(HTTPServer *server, int response_code, char *body)
{
  if (server == NULL)
    return -1;

  body = NULL;
  char *input = "{\"name\":\"Alice\",\"age\":30}";

  char response[1024];
  int length = snprintf(response, sizeof(response),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Length: %zu\r\n"
                        "Content-Type: application/json\r\n"
                        "\r\n"
                        "%s",
                        strlen(input), input);
  int result = HTTP_Write(server, response, length);
  return result;
}

void HTTP_work(void *_Context, uint64_t monTime)
{
  HTTPServer *server = (HTTPServer *)_Context;

  if (server == NULL)
    return;

  switch (server->status)
  {
  case http_server_initialized:
    server->status = http_server_awaiting_connection;
    break;
  case http_server_awaiting_connection:
    if (HTTP_Accept(server) >= 0)
    {
      printf("HTTP Server: Client connected\n");
      server->status = http_server_parse_header;
      break;
    }
    break;
  case http_server_connected:
    /*int result = HTTP_Read(server);
    if (result < 0)
    {
      printf("HTTP Server: Failed to read data\n");
      break;
    }
    server->status = http_server_POST;
    break;*/
  case http_server_parse_header:
    if (HTTP_ParseHeader(server) < 0)
    {
      printf("HTTP Server: Failed to parse header\n");
      // server->status = http_server_dispose;
      break;
    }
    server->status = http_server_POST;
    break;
  case http_server_POST:
    printf("HTTP Server: Handling POST request\n");
    if (HTTPServer_SendResponse(server, 200, NULL) < 0)
    {
      break;
    }
    server->status = http_server_dispose;
    break;
  case http_server_dispose:
    HTTP_Dispose(&server);
    server->status = http_server_initialized;
    break;
  }
}

void HTTP_Dispose(HTTPServer **server)
{
  if (server == NULL || *server == NULL)
    return;

  HTTPServer *_server = *server;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);

  tcpserver_dispose(_server->tcp_server);
  free(_server->url);
  free(_server);
  *server = NULL;
}
