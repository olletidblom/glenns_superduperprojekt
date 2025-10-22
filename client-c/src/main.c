#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char http_buffer[4096] = {0};
 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    char *hello = "Hello from client!";
    send(sock, hello, strlen(hello), 0);

    read(sock, http_buffer, 4096);
    printf("HTTP Response:\n%s\n", http_buffer);
    read(sock, buffer, 1024);
    printf("Server: %s\n", buffer);
    close(sock);
    return 0;
}