#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "../../libs/TCPClient.h"
#include "../../libs/smw.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


typedef void (*TCPServer_OnConnection)(void* context, int socket);

typedef struct {
  smw_task *task;
  TCPServer_OnConnection onConnect;
  void* context;
  int server_socket;
  int backlog;
  int port;
  TCPClient *client;
} TCPServer;

ssize_t tcpserver_recieve(TCPServer *server, void *buffer, size_t len);

ssize_t tcpserver_send(TCPServer *server, void *data, size_t len);

int tcpserver_listen(TCPServer *server, int port, int backlog, TCPServer_OnConnection callback, void* context);

int tcpserver_accept(TCPServer *server);


void tcpserver_disconnect(int socket); 

void tcpserver_dispose(TCPServer *server);

#endif
