#include "TCPServer.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t tcpserver_recieve(TCPServer *server, void *buffer, size_t len)
{
  int result = recv(server->client->client_socket, buffer, len, MSG_DONTWAIT);
  return result;
}

ssize_t tcpserver_send(TCPServer *server, void *data, size_t len)
{
  ssize_t result = send(server->client->client_socket, data, len, MSG_NOSIGNAL);
  return result;
}

void tcpserver_work(void* _Context, uint64_t monTime);

int tcpserver_listen(TCPServer *server, int port, int backlog, TCPServer_OnConnection callback, void* context)
{
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  char port_str[10];
  int sock = -1;
  int optval = 1;
  
  server->onConnect = callback;
  printf("Starting TCP server on port %d\n", port);
  server->context = context;
  server->port = port;
  server->backlog = backlog;

  server->client = malloc(backlog * sizeof(TCPClient));
  if (server->client == NULL)
    return -4;
  

  printf("PORT: %d\n", server->port);
  memset(&hints, 0, sizeof(hints));
  snprintf(port_str, sizeof(port_str), "%d", server->port);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port_str, &hints, &res) != 0)
  {
    printf("Failed to convert hostname\n");
    return -1;
  }

  for (struct addrinfo *temp = res; temp; temp = temp->ai_next)
  {
    sock = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) <
        0)
    {
      continue;
    }

    if ((bind(sock, temp->ai_addr, temp->ai_addrlen)) < 0)
    {
      printf("Failed to bind socket to port\n");
      //freeaddrinfo(res);
      close(sock);
      sock = -1;
      continue;
    }
    else
    {
      break;
    }
  }

  freeaddrinfo(res);

  if (listen(sock, server->backlog) < 0)
  {
    close(sock);
    return -3;
  }

  server->server_socket = sock;

  int flags = fcntl(server->server_socket, F_GETFL, 0);
  fcntl(server->server_socket, F_SETFL, flags | O_NONBLOCK);

  for (int i = 0; i < server->backlog; i++)
    server->client[i].client_socket = -1;

  server->task = smw_create_task(server, tcpserver_work);
  printf("Listening\n");
  return 0;
}

int tcpserver_accept(TCPServer *server)
{
  int sock = accept(server->server_socket, NULL, NULL);

  if (sock < 0)
  {
    if (errno == EWOULDBLOCK || errno == EAGAIN)
      return -1;

    perror("accept");
    return -1;
  }


  int result = server->onConnect(server->context, sock);

  if (result < 0)
  {
    printf("Connection callback failed\n");
    close(sock);
    return -1;
  }


  return 0;
}

void tcpserver_work(void* _Context, uint64_t monTime)
{
  TCPServer *server = (TCPServer *)_Context;

  if (server == NULL)
    return;

  // Accept new connections
  tcpserver_accept(server);
}

void tcpserver_disconnect(int socket)
{

  close(socket);
  socket = -1;

}

void tcpserver_dispose(TCPServer *server)
{
  if(server == NULL || server->server_socket == -1)
  return;

  if(server->task != NULL)
  smw_destroy_task(server->task);
  
  close(server->server_socket);

    free(server->client);
    server->client = NULL;
  
  free(server);

}
