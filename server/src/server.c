// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
// server.c - micro-server allowing client connection, expect a message and answers it
#include "server.h"
#include "../../libs/HTTP.h"
#include "../../libs/TCP.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>


#define PORT 8080  // Server port
#define BACKLOG 10 // maximum connection at a time

void server_dispose(server_s** srv);


bool server_receive(int socket)
{
    char buffer[BUFSIZ];
    int bytes_read;

    printf("Reading client socket %d\n", socket);
    bytes_read = recv(socket, buffer, BUFSIZ, 0);
    if (bytes_read == 0)
    {
        printf("Client socket %d: closed connection.\n", socket);
        return false;
    }
    else if (bytes_read == -1)
    {
        printf("recv error: %s\n", strerror(errno));
        return false;
    }
    else
    {
        // Si on a bien reçu un message, on va l'imprimer
        // puis renvoyer un message au client
        // If message recieved, print it and send a message to client
        char *msg = "Got your message.";
        int msg_len = strlen(msg);

        buffer[bytes_read] = '\0';
        printf("Message received from client socket %d: \"%s\"\n", socket, buffer);
    }
    return true;
}

void server_send(int socket, char* msg)
{

    int msg_len = strlen(msg);
    int bytes_sent;

    if(msg_len <= 0)
    {
        return;
    }

    bytes_sent = send(socket, msg, msg_len, 0);
    if (bytes_sent == -1)
    {
        printf("send error: %s\n", strerror(errno));
        return;
    }
    else if (bytes_sent == msg_len)
    {
        printf("Sent full message to client socket %d: \"%s\"\n", socket, msg);
    }
    else
    {
        printf("Sent partial message to client socket %d: %d bytes sent.\n", socket, bytes_sent);
    }
}


bool server_init(server_s** srv)
{
    if(srv == NULL)
    return false;

    server_s* _Srv = (server_s*)malloc(sizeof(server_s));

    if(_Srv == NULL)
    return false;

    _Srv->task = smw_create_task(_Srv, server);


    *srv = _Srv;
    return true;
}

void server(void* _Context)
{
    server_s* _Srv = (server_s*)_Context;
    printf("---- SERVER ----\n\n");
    char response_buffer[4096];

    const char *hostname = "example.com";
    const char *path = "/";

    int server_socket = tcp_listen(PORT, BACKLOG);
    int client_socket = tcp_accept(server_socket);

    char* msg = "Got you message.\n";

    /*while(server_receive(client_socket))
    {
        int status = HTTP_Get(hostname, path, response_buffer, sizeof(response_buffer));
        if(status != 0){
            printf("HTTP_Get failed with status: %d\n", status);
        }
        else{
            printf("HTTP_Get succeeded. Response:\n%s\n", response_buffer);
            server_send(client_socket, response_buffer);
        }

        server_send(client_socket, msg);
    }*/

    server_send(client_socket, msg);


    printf("Closing client socket\n");
    close(client_socket);
    printf("Closing server socket\n");
    close(server_socket);
    server_dispose(&_Srv);
}

int server_run(void (*_Callback)())
{
    server_s* srv = NULL;

    if(!server_init(&srv))
    return -1;

    srv->callback = _Callback;

    return 0;

}

void server_dispose(server_s** srv)
{
    if(srv == NULL || *srv == NULL)
    return;

    server_s* _Srv = *srv;

    if(_Srv->task != NULL)
    smw_destroy_task(_Srv->task);

    free(_Srv);
    *srv = NULL;
}