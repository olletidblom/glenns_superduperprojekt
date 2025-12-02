#include "TCPClient.hpp"

int TCPClient::TCPClient_Connect()
{
    struct addrinfo hints = {0};
    struct addrinfo *res;
    std::string port;
    int sock = -1;

    port = std::to_string(this->port);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(this->hostname.c_str(), port.c_str(), &hints, &res) != 0)
    {
        printf("Failed to convert hostname\n");
        return -1;
    }

    for (struct addrinfo *temp = res; temp; temp = temp->ai_next)
    {
        sock = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

        if(sock < 0)
        {
            continue;
        }

        if ((connect(sock, temp->ai_addr, temp->ai_addrlen)) == 0)
        {
            printf("Successfully connected\n");
            break;
        }

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    if (sock < 0)
        return -3;

    this->client_socket = sock;
    return 0;
}

int TCPClient::TCPClient_Send(const std::string &message)
{
    ssize_t bytesSent = send(this->client_socket, message.c_str(), message.size(), 0);

    printf("Message sent: %s\n", message.c_str());
    if (bytesSent <= 0)
    {
        std::printf("Failed to send data!\n");
        return -1;
    }

    return 0;
}

std::string TCPClient::TCPClient_Recieve()
{
    char buffer[2048];
    ssize_t bytesReceived = recv(this->client_socket, buffer, sizeof(buffer), 0);

    if (bytesReceived <= 0)
    {
        std::printf("Failed to receive data!\n");
        return {};
    }
    std::string result = std::string(buffer);

    return result;
}