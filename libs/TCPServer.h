#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define _POSIX_C_SOURCE 200809L
#include "TCPClient.h"
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>


typedef struct 
{
    int server_socket;
    int backlog;
    int port;
    TCPClient client[10];
}TCPServer;


int tcpserver_listen(TCPServer* server, int port, int backlog);

int tcpserver_accept(TCPServer* server);

void tcpserver_work(TCPServer* server);

void tcpserver_dispose(TCPServer* server);

#endif