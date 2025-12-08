#include "HTTPServer.h"
#include "Handlers/handler.h"
#include "Handlers/weather_handler.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>



int is_active = 0;
int HTTPServer_OnConnect(void* _Context, int socket);
int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length);

void HTTPServer_work(void *_Context, uint64_t monTime);




int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server)
{

  //HTTP_Status status;
  TCPServer* tcp_server = (TCPServer *)malloc(sizeof(TCPServer));
  HTTPServer *http_server = (HTTPServer *)malloc(sizeof(HTTPServer));

  http_server->task = smw_create_task(http_server, HTTPServer_work);
  http_server->status = http_server_initialized;
  http_server->method = method;
  http_server->tcp_server = tcp_server;

  
  HTTPServer_RegisterRoute("gwd", Handle_Weather);
  HTTPServer_RegisterRoute("geo", Handle_GEO);

  Cities_Init(&http_server->cities);

  printf("Initializing TCP server\n");
  tcpserver_listen(tcp_server, 10180, 1000, HTTPServer_OnConnect, http_server);

  *server = http_server;
  return 0;
}




//Gets called every time a new connection is received
int HTTPServer_OnConnect(void* _Context, int socket)
{
  printf("HTTP Server: Connection received on socket %d\n", socket);
  HTTPServer* server = (HTTPServer *)_Context;

  if (server == NULL)
    return -1;

  server->connection = NULL;
  if( HTTPServerConnection_Initialize(&server->connection, socket, &is_active) < 0) // added , server
  {
    printf("Failed to initialize HTTP server connection\n");
    return -2;
  }

  HTTPServerHandler* handler = NULL;

  if(HTTPServerHandler_Initialize(&handler, server->connection, HTTPServer_FindRoute) < 0)
  {
    printf("Failed to initialize handler\n");
    return -3;
  }

  return 0;
}

void HTTPServer_work(void *_Context, uint64_t monTime)
{
  HTTPServer *server = (HTTPServer *)_Context;

  if (server == NULL || server->connection == NULL)
    return;

  if(is_active == 1)
  HTTPServer_Dispose(&server);

}

void HTTPServer_Dispose(HTTPServer **server)
{
  if (server == NULL || *server == NULL)
    return;

  HTTPServer *_server = *server;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);
  
  

  Cities_Dispose(&_server->cities);
  HTTPServer_RouteCleanup();
  tcpserver_dispose(_server->tcp_server);
  free(_server);
  *server = NULL;
}
