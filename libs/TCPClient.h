#ifndef TCP_H
#define TCP_H

#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#define INIT_BUFFER_SIZE 10000

typedef struct
{
    int client_socket;
    const char* hostname;
    int port;
}TCPClient;


int tcp_init(TCPClient* client, const char* hostname, int port);

int tcp_connect(TCPClient* client);

ssize_t tcp_send(TCPClient* client, const void* data, size_t len);

int tcp_sendAll(TCPClient* client, const void* data, size_t len, uint64_t timeout);

ssize_t tcp_recieve(TCPClient* client, void* buffer, size_t len);

int tcp_recieveAll(TCPClient* client, char** msg, uint64_t timeout);

void tcp_disconnect(TCPClient* client);

void tcp_dispose(TCPClient* client);

#endif