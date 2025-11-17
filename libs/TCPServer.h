#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPClient.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
  int server_socket;
  int backlog;
  int port;
  TCPClient client[10];
} TCPServer;

ssize_t tcpserver_recieve(TCPServer *server, void *buffer, size_t len);

ssize_t tcpserver_send(TCPServer *server, void *data, size_t len);

int tcpserver_listen(TCPServer *server, int port, int backlog);

int tcpserver_accept(TCPServer *server);


void tcpserver_disconnect(TCPServer *server, int socket); 

void tcpserver_dispose(TCPServer *server);

#endif
