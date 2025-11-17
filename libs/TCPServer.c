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

ssize_t tcpserver_recieve(TCPServer *server, void *buffer, size_t len) {
  return recv(server->server_socket, buffer, len, MSG_DONTWAIT);
}

ssize_t tcpserver_send(TCPServer *server, void *data, size_t len) {
  ssize_t result = send(server->server_socket, data, len, MSG_NOSIGNAL);
  //printf("send error %d: %s\n", errno, strerror(errno));
  //scanf("%d", result);
  return result;
}

int tcpserver_listen(TCPServer *server, int port, int backlog) {
  struct addrinfo hints;
  struct addrinfo *res = NULL;
  char port_str[10];
  int sock = -1;
  int optval = 1;
  server->port = port;
  server->backlog = backlog;

  printf("PORT: %d\n", server->port);
  memset(&hints, 0, sizeof(hints));
  snprintf(port_str, sizeof(port_str), "%d", server->port);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port_str, &hints, &res) != 0) {
    printf("Failed to convert hostname\n");
    return -1;
  }

  for (struct addrinfo *temp = res; temp; temp->ai_next) {
    sock = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) <
        0) {
      continue;
    }

    if ((bind(sock, temp->ai_addr, temp->ai_addrlen)) < 0) {
      printf("Failed to bind socket to port\n");
      freeaddrinfo(res);
      close(sock);
      return -2;
    } else {
      break;
    }
  }

  freeaddrinfo(res);

  if (listen(sock, server->backlog) < 0) {
    close(sock);
    return -3;
  }

  server->server_socket = sock;

  int flags = fcntl(server->server_socket, F_GETFL, 0);
  fcntl(server->server_socket, F_SETFL, flags | O_NONBLOCK);

  for (int i = 0; i < server->backlog; i++)
    server->client[i].client_socket = -1;

  printf("Listening\n");
  return 0;
}

int tcpserver_accept(TCPServer *server) {
  int sock = accept(server->server_socket, NULL, NULL);

  if (sock < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN)
      return -1;

    perror("accept");
    return -1;
  }

  for (int i = 0; i < server->backlog; i++) {
    if (server->client[i].client_socket < 0) {
      server->client[i].client_socket = sock;
      printf("Connected on socket %d\n", server->client[i].client_socket);
      return 0;
    }
  }

  printf("No space for more clients\n");
  close(sock);
  return -2;
}

void tcpserver_work(TCPServer *server) {
  tcpserver_accept(server);

  char buffer[512];
  for (int i = 0; i < server->backlog; i++) {
    int sock = server->client[i].client_socket;
    if (sock < 0)
      continue;

    ssize_t n = recv(sock, buffer, sizeof(buffer), MSG_DONTWAIT);
    if (n > 0) {
      printf("Response: %s", buffer);
      send(sock, buffer, (size_t)n, MSG_NOSIGNAL);
    } else if (n == 0) {
      printf("Client %d disconnected\n", i);
      close(sock);
      server->client[i].client_socket = -1;
    }
  }
}

void tcpserver_dispose(TCPServer *server) {
  close(server->server_socket);

  for (int i = 0; i < server->backlog; i++) {
    close(server->client[i].client_socket);
  }
}
