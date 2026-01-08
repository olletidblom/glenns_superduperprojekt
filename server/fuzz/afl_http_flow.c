#include "HTTPServerConnection_fuzz.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char **argv)
{

    HTTPRequest request;
    HTTPRequest_Initialize(&request);

    URLHandler* handler;
    URLHandler_Initialize(&handler);



    

    ssize_t size = read(STDIN_FILENO, request.recv_buffer, sizeof(request.recv_buffer));
    if (size <= 0)
    {
        return 0;
    }


    request.recv_buffer_length = (size_t)size;


    HTTPRequest_ParseHeader(&request);

    if (request.url)
    {
        URLHandler_Parse(handler,
                         request.url);
    }


    URLHandler_Dispose(&handler);
    return 0;
}
