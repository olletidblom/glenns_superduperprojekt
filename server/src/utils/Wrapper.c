
#include "Wrapper.h"
#include <sys/socket.h>
#include <sys/types.h>



int HTTPConnection_Read(int socket, uint8_t *buffer, size_t length)
{
    return recv(socket, buffer, length, MSG_DONTWAIT);
}

int HTTPConnection_Write(int socket, char *buffer, size_t length)
{
    ssize_t result = send(socket, buffer, length, MSG_NOSIGNAL);
    return result;
}