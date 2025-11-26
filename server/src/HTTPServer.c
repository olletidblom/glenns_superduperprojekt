#include "HTTPServer.h"
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
void HTTPServer_OnConnect(void* _Context, int socket);
int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length);

void HTTPServer_work(void *_Context, uint64_t monTime);

void HTTPServer_RemoveConnection(HTTPServer *server, HTTPServerConnection *connection);
void HTTPServer_Disconnect(HTTPServer *server);


int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server)
{

  HTTP_Status status;
  TCPServer* tcp_server = (TCPServer *)malloc(sizeof(TCPServer));
  HTTPServer *http_server = (HTTPServer *)malloc(sizeof(HTTPServer));

  http_server->task = smw_create_task(http_server, HTTPServer_work);
  http_server->status = http_server_initialized;
  http_server->method = method;
  http_server->tcp_server = tcp_server;

  HTTPServer_RegisterRoute("gwd", Handle_Weather);

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
  if( HTTPServerConnection_Initialize(&server->connection, socket, server, &is_active) < 0) // added , server
  {
    printf("Failed to initialize HTTP server connection\n");
    return;
  }
  printf("HELLOHELLO %p\n", server->connection);
  HTTPServerHandler* handler = NULL;

  if(HTTPServerHandler_Initialize(&handler, server->connection, HTTPServer_FindRoute) < 0)
  {
    printf("Failed to initialize handler\n");
    return;
  }


}

int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length)
{
  int result = tcpserver_send(server->tcp_server, (char *)buffer, length);

  printf("message sent: %s\n", buffer);
  return result;
}

int HTTPServer_Read(HTTPServer *server, uint8_t *buffer, size_t length)
{
  int result = tcpserver_recieve(server->tcp_server, buffer, length);

  return result;
}


void HTTPServer_work(void *_Context, uint64_t monTime)
{
  HTTPServer *server = (HTTPServer *)_Context;

  if (server == NULL || server->connection == NULL)
    return;

  printf("dsadsa %d\n", is_active);
  if(is_active == 1)
  HTTPServer_Dispose(&server);

}

void HTTPServer_RemoveConnection(HTTPServer *server, HTTPServerConnection *connection)
{
    if (server == NULL || connection == NULL)
    return;

    printf("HTTPServer: Removing connection on socket %d\n", connection->socket);

    HTTPServerConnection_Dispose(&connection);
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
  HTTPServer_RouteCleanup();
  tcpserver_dispose(_server->tcp_server);
  free(_server);
  *server = NULL;
}
