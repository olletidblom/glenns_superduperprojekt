#include "HTTPServer.h"
#include "utils.h"
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

void strtokRepeat(char *str, int repetition) {
  char *myPtr = strtok(str, " ");
  int i;
  for (i = 0; i < (repetition - 1); i++) {
    myPtr = strtok(NULL, " ");
  }
}

int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server) {
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

int HTTP_Accept(HTTPServer *server) {
  int result = tcpserver_accept(server->tcp_server);

  if (result < 0)
    printf("HTTP: Failed to connect to client\n");

  return result;
}

int HTTP_Write(HTTPServer *server, char *buffer, size_t length) {
  /*int result = tcp_sendAll(server->tcp_server, buffer, length, 5000);

  if (result < 0)
    printf("HTTP: Failed to send data\n");

  return result;*/
  return 0;
}

int HTTP_Read(HTTPServer *server) {
  char buffer[512];
  int result = tcpserver_recieve(server->tcp_server, buffer, sizeof(buffer));

  if (result < 0)
    printf("HTTP: Failed to receive data\n");
  server->HTTP_response = strndup(buffer, result);
  return result;
}

int HTTP_ParseHeader(HTTPServer *server) {
  /*
  char *myPtr = strtok(myStr, " ");
  myPtr = strtok(NULL, " ");
  */

  if (server == NULL)
    return -1;

  if (server->HTTP_response == NULL)
    return -2;

  char *myStr =
      strndup(server->HTTP_response, strlen(server->HTTP_response) + 1);
  printf("%s", myStr);

  if (myStr == NULL)
    return -3;

  char *myPtr = strtok(myStr, " ");

  myPtr = strtok(NULL, " ");

  server->response_code = strtol(myPtr, NULL, 10);

  if (server->response_code == 0) {
    printf("Failed to get response code\n");
    free(myStr);
    return -4;
  }

  printf("Response code: %d\n", server->response_code);
  free(myStr);

  return 0;
}

int HTTP_Post(HTTPServer *server) {
  char *data = "{\"key1\":\"value1\",\"key2\":\"value2\"}";
  size_t data_length = strlen(data);

  char request[1024];
  snprintf(request, sizeof(request),
           "POST / HTTP/1.1\r\n"
           "HOST: %s\r\n"
           "Content-Type: application/json\r\n"
           "Content-Length: %zu\r\n"
           "Connection: close\r\n"
           "\r\n"
           "%s",
           "localhost", data_length, data);

  int write_result = HTTP_Write(server, request, strlen(request));

  if (write_result < 0) {
    printf("Failed to send request\n");
    close(server->tcp_server->server_socket);
    return -1;
  }
}

void HTTP_work(void *_Context, uint64_t monTime) {
  HTTPServer *server = (HTTPServer *)_Context;

  static uint64_t *super_cool_timer = NULL;

  if (server == NULL)
    return;

  switch (server->status) {
  case http_server_initialized:
    server->status = http_server_awaiting_connection;
    break;
  case http_server_awaiting_connection:
    if (HTTP_Accept(server) == 0) {
      printf("HTTP Server: Client connected\n");
      server->status = http_server_connected;
      break;
    }
    break;
  case http_server_connected:
    int result = HTTP_Read(server);
    if (!super_cool_timer)
      *super_cool_timer = SystemMonotonicMS();
    if (result < 0 && *super_cool_timer > (SystemMonotonicMS() - 5000)) {
      printf("HTTP Server: Failed to read data\n");
      break;
    }
    server->status = http_server_parse_header;
    break;
  case http_server_parse_header:
    if (HTTP_ParseHeader(server) < 0) {
      printf("HTTP Server: Failed to parse header\n");
      server->status = http_server_dispose;
      break;
    }
    server->status = http_server_POST;
    break;
  case http_server_POST:
    if (HTTP_Post(server) < 0) {
      printf("HTTP Server: Failed to handle POST request\n");
      server->status = http_server_dispose;
      break;
    }
    server->status = http_server_dispose;
    break;
  case http_server_dispose:
    HTTP_Dispose(&server);
  }
}

void HTTP_Dispose(HTTPServer **server) {
  if (server == NULL || *server == NULL)
    return;

  HTTPServer *_server = *server;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);

  tcpserver_dispose(_server->tcp_server);
  free(_server->HTTP_response);
  free(_server);
  *server = NULL;
}
