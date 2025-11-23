#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPClient.h"
#include "smw.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>



typedef void (*TCPServer_OnConnection)(void* context, int socket);

typedef struct {
  smw_task *task;
  TCPServer_OnConnection onConnect;
  void* context;
  int server_socket;
  int backlog;
  int port;
  TCPClient client[10];
} TCPServer;

ssize_t tcpserver_recieve(int socket, void *buffer, size_t len);

ssize_t tcpserver_send(int socket, void *data, size_t len);

int tcpserver_listen(TCPServer *server, int port, int backlog, TCPServer_OnConnection callback, void* context);

int tcpserver_accept(TCPServer *server);


void tcpserver_disconnect(int socket); 

void tcpserver_dispose(TCPServer *server);

#endif
