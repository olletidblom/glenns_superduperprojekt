#include "HTTPParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int HTTPParser_ParseHeader(HTTPRequest* request) // Returns: 0 on success, -1 on error, 1 if needs more data
{
    if (request == NULL)
        return -1;

    char *header_end = strstr(request->recv_buffer, "\r\n\r\n");
    if (header_end == NULL)
    {
        return 1;
    }

    size_t header_size = (header_end + 4) - request->recv_buffer;
    if (header_size > 1024)
    {
        printf("HTTPServerConnection_ParseHeader: Headers too large (buffer full)\n");
        return -1;
    }

    char *check_ptr = request->recv_buffer;
    while (check_ptr < request->recv_buffer + request->recv_buffer_length)
    {
        if (*check_ptr == '\n')
        {
            if (check_ptr == request->recv_buffer || *(check_ptr - 1) != '\r')
            {
                printf("HTTPServerConnection_ParseHeader: Invalid line ending (bare LF without CR)\n");
                return -1;
            }
        }
        check_ptr++;
    }

    if (strncmp(request->recv_buffer, "GET ", 4) != 0 && strncmp(request->recv_buffer, "OPTIONS ", 8) != 0)
    {
        printf("HTTPServerConnection_ParseHeader: Unsupported HTTP method\n");
        return -1;
    }

    char *path_start = strchr(request->recv_buffer, ' ');
    if (path_start == NULL)
    {
        return -1;
    }

    path_start++;
    char *path_end = strchr(path_start, ' ');
    if (path_end == NULL)
    {
        return -1;
    }

    request->url = strndup(path_start, path_end - path_start);
    if (request->url == NULL)
    {
        printf("HTTPServerConnection_ParseHeader: Failed to copy url\n");
        return -1;
    }

    return 0;
}