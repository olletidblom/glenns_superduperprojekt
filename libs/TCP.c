#include "TCP.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>




int tcp_connect(const char* hostname, int port)
{
    int remote_socket;
    struct sockaddr_in remote_sa;
    struct hostent* hostent;
    
    hostent = gethostbyname(hostname);
    if(hostent == NULL){
        fprintf(stderr, "gethostname error: %s\n", strerror(errno));
        return (1);
    }

    memset(&remote_sa, 0, sizeof remote_sa);
    remote_sa.sin_family = AF_INET; 
    remote_sa.sin_port = htons(80);
    memcpy(&remote_sa.sin_addr, hostent->h_addr_list[0], hostent->h_length);

    remote_socket = socket(remote_sa.sin_family, SOCK_STREAM, 0);
    if (remote_socket == -1) {
        fprintf(stderr, "socket fd error: %s\n", strerror(errno));
        return (2);
    }

    if(connect(remote_socket, (struct sockaddr*)&remote_sa, sizeof remote_sa) == -1){
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return (3);
    }

    return remote_socket;
}


ssize_t tcp_send(int socket, const void* data, size_t len)
{
    return send(socket, data, len, 0);
}

ssize_t tcp_recieve(int socket, void* buffer, size_t len)
{
    return recv(socket, buffer, len, 0);
}


void tcp_close(int socket)
{
    close(socket);
}


int tcp_listen(int port, int backlog)
{
    struct sockaddr_in sa;
    int server_socket;
    int status;


    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; 
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_port = htons(port);

        server_socket = socket(sa.sin_family, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "socket fd error: %s\n", strerror(errno));
        return (1);
    }
    printf("Created server socket fd: %d\n", server_socket);

    status = bind(server_socket, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        return (2);
    }
    printf("Bound socket to localhost port %d\n", port);

    printf("Listening on port %d\n", port);
    status = listen(server_socket, backlog);
    if (status != 0) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        return (3);
    }


    return server_socket;
}

int tcp_accept(int socket)
{
    socklen_t addr_size;
    struct sockaddr_storage client_addr;
    int client_socket;

    addr_size = sizeof client_addr;
    client_socket = accept(socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket == -1) {
        fprintf(stderr, "client fd error: %s\n", strerror(errno));
        return (4);
    }
    printf("Accepted new connection on client socket fd: %d\n", client_socket);

    return client_socket;
}