// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
// server.c - micro-server allowing client connection, expect a message and answers it
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
        fprintf(stderr, "recv error: %s\n", strerror(errno));
        return false;
    }
    else
    {
        // Si on a bien reçu un message, on va l'imprimer
        // puis renvoyer un message au client
        // If message recieved, print it and send a message to client
        char *msg = "Got your message.";
        int msg_len = strlen(msg);
        int bytes_sent;

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
        fprintf(stderr, "send error: %s\n", strerror(errno));
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


int main(void)
{

    printf("---- SERVER ----\n\n");
    char response_buffer[4096];

    const char *hostname = "example.com";
    const char *path = "/";

    int server_socket = tcp_listen(PORT, BACKLOG);
    int client_socket = tcp_accept(server_socket);

    char* msg = "Got you message.\n";

    while(server_receive(client_socket))
    {
        int status = HTTP_Get(hostname, path, response_buffer, sizeof(response_buffer));
        if(status != 0){
            fprintf(stderr, "HTTP_Get failed with status: %d\n", status);
        }
        else{
            printf("HTTP_Get succeeded. Response:\n%s\n", response_buffer);
            server_send(client_socket, response_buffer);
        }

        server_send(client_socket, msg);
    }


    printf("Closing client socket\n");
    close(client_socket);
    printf("Closing server socket\n");
    close(server_socket);

    return (0);
}