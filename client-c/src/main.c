#include "../../libs/TCPClient.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    TCPClient client;
    const char* msg = "Hello Client!\n";
    tcp_init(&client, "localhost", 8080);

    if((tcp_connect(&client)) < 0)
    {
        return -1;
    }

    tcp_sendAll(&client, msg, strlen(msg) + 1, 5000);

    tcp_recieveAll(&client, strlen(msg) + 1, 5000);
    

    tcp_dispose(&client);
    return 0;
}