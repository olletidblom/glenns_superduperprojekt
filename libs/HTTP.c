#include "HTTP.h"
#include "TCPClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>




int HTTP_Get(const char* hostname, const char* path, char* buffer, size_t buffer_size)
{
    int http_socket = tcp_connect(NULL);

    char request[1024];
    snprintf(request, sizeof(request), "GET %s HTTP/1.0\r\n" "Host: %s\r\n" "Connection: close\r\n" "\r\n",
             path, hostname);

    ssize_t bytes_sent = tcp_send(http_socket, request, strlen(request));
    if (bytes_sent == -1) {
        fprintf(stderr, "send error: %s\n", strerror(errno));
        return (4);
    }

    ssize_t bytes_received;
    size_t total_bytes_received = 0;
    ssize_t old_bytes_received = 0;

    while(1){
        bytes_received = tcp_recieve(http_socket, buffer + total_bytes_received, buffer_size - total_bytes_received - 1);
        if(bytes_received <= 0){
            break;
        }
        if(bytes_received == old_bytes_received){
            break;
        }
        total_bytes_received += bytes_received;
        old_bytes_received = bytes_received;
    }

    buffer[total_bytes_received] = '\0';
    close(http_socket);
    return 0;
}