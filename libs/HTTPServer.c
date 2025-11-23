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

int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length);

void HTTPServer_work(void *_Context, uint64_t monTime);

void HTTPServer_Disconnect(HTTPServer *server);

void HTTPServer_OnConnect(void* _Context, int socket);

int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server)
{

  TCPServer* tcp_server = (TCPServer *)malloc(sizeof(TCPServer));

  HTTPServer *http_server = (HTTPServer *)malloc(sizeof(HTTPServer));

  curl_global_init(CURL_GLOBAL_ALL);

  http_server->task = smw_create_task(http_server, HTTPServer_work);
  http_server->method = method;

  printf("Initializing TCP server\n");
  tcpserver_listen(tcp_server, 10180, 1000, HTTPServer_OnConnect, http_server);

  *server = http_server;
  return 0;
}


void HTTPServer_OnConnect(void* _Context, int socket)
{
  printf("HTTP Server: Connection received on socket %d\n", socket);
  HTTPServer* server = (HTTPServer *)_Context;

  if (server == NULL)
    return;
  


  server->connection = NULL;
  if( HTTPServerConnection_Initialize(&server->connection, socket, Request_ServerHandleRequest, API_ProcessRequest, Curl_HTTPGetServer) < 0)
  {
    printf("Failed to initialize HTTP server connection\n");
    return;
  }

}

void HTTPServer_work(void *_Context, uint64_t monTime)
{
  HTTPServer *server = (HTTPServer *)_Context;

  if (server == NULL)
    return;
  
  if(server->connection == NULL)
  return;

  //if(server->connection != NULL){
  //if(server->connection->state == state_dispose)
  //  HTTPServerConnection_Dispose(&server->connection);
  //}
}

void HTTPServer_Disconnect(HTTPServer *server)
{
  if (server == NULL)
    return;

  //tcpserver_disconnect(server->tcp_server, server->tcp_server->client->client_socket);
}

void HTTPServer_Dispose(HTTPServer **server)
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
