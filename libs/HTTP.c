/*#include "HTTP.h"
#include "TCPClient.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// int HTTP_work(void *_Context);

int HTTP_Read(HTTPClient *client);

int HTTP_Write(HTTPClient *client, char *buffer, size_t length);

void HTTP_Dispose(HTTPClient **client);

void strtokRepeat(char *str, int repetition) {
  char *myPtr = strtok(str, " ");
  int i;
  for (i = 0; i < (repetition - 1); i++) {
    myPtr = strtok(NULL, " ");
  }
}

int HTTP_Initialize(HTTP_Method method, HTTPClient **client) {
  TCPClient *tcp_client = (TCPClient *)malloc(sizeof(TCPClient));

  tcp_init(tcp_client, "localhost", 10180);

  if (client == NULL || tcp_client->hostname == NULL)
    return -1;

  HTTP_Status status;
  HTTPClient *http_client = (HTTPClient *)malloc(sizeof(HTTPClient));

  http_client->task = smw_create_task(http_client, HTTP_work);
  http_client->status = http_client_initialized;
  http_client->method = method;

  http_client->tcp_client = tcp_client;

  *client = http_client;
  return 0;
}

int HTTP_Get(HTTPClient *client) {
  char request[1024];
  snprintf(request, sizeof(request),
           "GET / HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Connection: close\r\n"
           "\r\n",
           client->tcp_client->hostname);

  int write_result = HTTP_Write(client, request, strlen(request));

  if (write_result < 0) {
    printf("Failed to send request\n");
    close(client->tcp_client->client_socket);
    return -1;
  }

  int result = HTTP_Read(client);

  if (result < 0) {
    printf("GET request failed\n");
    close(client->tcp_client->client_socket);
    return -2;
  }

  printf("Response received:\n%s\n", client->HTTP_response);

  close(client->tcp_client->client_socket);
  return 0;
}

int HTTP_Connect(HTTPClient *client) {
  int result = tcp_connect(client->tcp_client);

  if (result < 0)
    printf("HTTP: Failed to connect\n");

  return result;
}

int HTTP_Write(HTTPClient *client, char *buffer, size_t length) {
  int result = tcp_send(client->tcp_client, buffer, length);

  if (result < 0)
    printf("HTTP: Failed to send data\n");

  return result;
}

int HTTP_Read(HTTPClient *client) {
  int result = tcp_recieve(client->tcp_client, &client->HTTP_response, 5000);

  if (result < 0)
    printf("HTTP: Failed to receive data\n");

  return result;
}

int HTTP_ParseHeader(HTTPClient *client) {
  if (client == NULL)
    return -1;

  char *myStr =
      strndup(client->HTTP_response, strlen(client->HTTP_response) + 1);
  if (myStr == NULL)
    return -1;

  char *myPtr = strtok(myStr, " ");
  myPtr = strtok(NULL, " ");
  // strtokRepeat(myStr, 2);
  client->response_code = strtol(myPtr, NULL, 10);
  if (client->response_code == 0) {
    printf("Failed to get response code\n");
    free(myStr);
    return -2;
  }

  printf("Response code: %d\n", client->response_code);
  free(myStr);

  return 0;
}

void HTTP_work(void *_Context, uint64_t monTime) {
  HTTPClient *client = (HTTPClient *)_Context;

  if (client == NULL)
    return;

  switch (client->status) {
  case http_client_initialized:
    client->status = http_client_connected;
    break;
  case http_client_connected:
    if (HTTP_Connect(client) < 0) {
      return; // http_failed_to_connect;
    }
    if (client->method == http_POST) {
      client->status = http_client_POST;
    } else {
      client->status = http_client_GET;
    }
    break;
  case http_client_POST:

    client->status = http_client_header;
  case http_client_GET:
    if (HTTP_Get(client) < 0) {
      return; // http_failed_to_read;
    }
    client->status = http_client_parse_header;
    break;
  case http_client_parse_header:
    HTTP_ParseHeader(client);
    client->status = http_client_header;
    break;
  case http_client_header:
    client->status = http_client_dispose;
    break;
  case http_client_dispose:
    HTTP_Dispose(&client);
    break;
  }
}

HTTPClient *HTTP_run(HTTP_Method method, void (*_Callback)()) {
  HTTPClient *client = NULL;
  if (!HTTP_Initialize(method, &client))
    return NULL;

  client->callback = _Callback;

  return client;
}

void HTTP_Dispose(HTTPClient **client) {
  if (client == NULL || *client == NULL)
    return;

  HTTPClient *_Client = *client;

  if (_Client->task != NULL)
    smw_destroy_task(_Client->task);

  tcp_dispose(_Client->tcp_client);
  free(_Client->HTTP_response);
  free(_Client);
  *client = NULL;
}*/
