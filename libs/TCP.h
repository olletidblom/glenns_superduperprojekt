#ifndef TCP_H
#define TCP_H

#include <stddef.h>
#include <sys/types.h>

int tcp_connect(const char* hostname, int port);

ssize_t tcp_send(int socket, const void* data, size_t len);

ssize_t tcp_recieve(int socket, void* buffer, size_t len);

void tcp_close(int socket);


//server functions
int tcp_listen(int port, int backlog);

int tcp_accept(int socket);


#endif