#include "TCPClient.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>


int tcp_init(TCPClient* client, const char* hostname, int port)
{
    client->client_socket = -1;
    client->hostname = hostname;
    client->port = port;
    return 0;
}

int tcp_connect(TCPClient* client)
{
    struct addrinfo hints = {0};
    struct addrinfo* res = NULL;
    char port_str[10];
    int sock = -1;


    memset(&hints, 0, sizeof(hints));
    snprintf(port_str, sizeof(port_str), "%d", client->port);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    if(getaddrinfo(client->hostname, port_str, &hints, &res) != 0)
    {
        printf("Failed to convert hostname\n");
        return -1;
    }

    for(struct addrinfo* temp = res; temp; temp = temp->ai_next)
    {
        sock = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

        if((connect(sock, temp->ai_addr, temp->ai_addrlen)) < 0)
        {
            printf("Failed to connect\n");
            freeaddrinfo(res);
            close(sock);
            return -2;
        }
        else
        {
            printf("Successfully connected\n");
            break;
        }
    }
    printf("1234\n");
    freeaddrinfo(res);
    printf("5678\n");
    if(sock < 0)
    return -3;

    
    

    client->client_socket = sock;
    printf("fdsfdsfds\n");
    return 0;
}


ssize_t tcp_send(TCPClient* client, const void* data, size_t len)
{
    return send(client->client_socket, data, len, 0);
}

int tcp_sendAll(TCPClient* client, const void* data, size_t len, uint64_t timeout)
{
    const uint8_t* ptr = (const uint8_t*)data;
    int bytesToWrite = len;
    uint64_t timer = SystemMonotonicMS();


    while(bytesToWrite > 0)
    {
        int bytesWritten = tcp_send(client, ptr, bytesToWrite);

        if(bytesWritten < 0)
        {
            printf("Failed to write\n");
            return -1;
        }

        if(bytesWritten == 0)
        {
            continue;
        }

        uint64_t timeDif = (SystemMonotonicMS() - timer);

        if(timeDif>= timeout)
        {
            printf("TIMED OUT\n");
            return -2;
        }

        ptr += bytesWritten;
        bytesToWrite -= bytesWritten;

    }

    return 0;

}


ssize_t tcp_recieve(TCPClient* client, void* buffer, size_t len)
{
    return recv(client->client_socket, buffer, len, 0);
}

int tcp_recieveAll(TCPClient* client, char** msg, uint64_t timeout)
{
    printf("READIN\n");
    uint8_t* ptr = (uint8_t*)malloc(INIT_BUFFER_SIZE);
    int bytesToRead = INIT_BUFFER_SIZE;
    uint64_t timer = SystemMonotonicMS();
    uint8_t* temp = ptr;

    while(bytesToRead > 0)
    {
        int bytesRead = tcp_recieve(client, temp, bytesToRead);

        if(bytesRead < 0)
        {
            printf("Failed to read\n");
            free(ptr);
            return -1;
        }

        if(bytesRead == 0)
        {
            break;
        }

        uint64_t timeDif = (SystemMonotonicMS() - timer);

        if(timeDif>= timeout)
        {
            printf("TIMED OUT\n");
            return -2;
        }
        temp += bytesRead;
        bytesToRead -= bytesRead;
    }


    *temp = '\0';
    *msg = (char*)ptr;
    return 0;
}

void tcp_disconnect(TCPClient* client)
{
    close(client->client_socket);
}

void tcp_dispose(TCPClient* client)
{
    tcp_disconnect(client);
    client->client_socket = -1;
    client->hostname = NULL;
    client->port = 0;
    client = NULL;
}


