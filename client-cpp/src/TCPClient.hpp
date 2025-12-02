#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP

#include <memory>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

class TCPClient
{
private:
    std::string hostname;
    int port;
    int client_socket;

public:
    TCPClient(const std::string &_Host, int _Port, int _ClientSocket) : hostname(_Host), port(_Port), client_socket(_ClientSocket)
    {
    }

    int TCPClient_Connect();

    int TCPClient_Send(const std::string& message);

    std::string TCPClient_Recieve();



    ~TCPClient()
    {
        close(client_socket);
    }
};

#endif