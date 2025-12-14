#include "HTTPRequest.h"
#include "Wrapper.h"
#include <stdint.h>
#include <errno.h>
#include <string.h>


int HTTPRequest_Initialize(HTTPRequest* http_request)
{
    if(http_request == NULL)
    return -1;

    http_request->content_length = 0;
    http_request->recv_buffer[0] = '\0';
    http_request->url = NULL;
    http_request->request_body = NULL;
}



int HTTPRequest_ReadHeaders(int socket, HTTPRequest *http_request)
{
    int bytesRead = HTTPConnection_Read(socket, (uint8_t*)(http_request->recv_buffer + http_request->recv_buffer_length),
                                        sizeof(http_request->recv_buffer) - http_request->recv_buffer_length - 1);

    if (bytesRead < 0)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return 1;
        }

        return -1;
    }

    if (bytesRead == 0)
    {
        // http_request->state = HTTPServerConnection_State_Cleanup;
        return 0;
    }

    http_request->recv_buffer_length += bytesRead;
    http_request->recv_buffer[http_request->recv_buffer_length] = '\0';

    if (strstr(http_request->recv_buffer, "\r\n\r\n") != NULL)
    {
        return 0;
    }

    return 1;
}



int HTTPRequest_ParseHeader(HTTPRequest* http_request)
{
    if (http_request == NULL)
        return -1;

    char *header_end = strstr(http_request->recv_buffer, "\r\n\r\n");
    if (header_end == NULL)
    {
    return 1; 
    }

    size_t header_size = (header_end + 4) - http_request->recv_buffer;
    if (header_size > 1024)
    {
        printf("HTTPServerConnection_ParseHeader: Headers too large (buffer full)\n");
        return -1;
    }

    char *check_ptr = http_request->recv_buffer;
    while (check_ptr < http_request->recv_buffer + http_request->recv_buffer_length)
    {
        if (*check_ptr == '\n')
        {
            if (check_ptr == http_request->recv_buffer || *(check_ptr - 1) != '\r')
            {
                printf("HTTPServerConnection_ParseHeader: Invalid line ending (bare LF without CR)\n");
                return -1;
            }
        }
        check_ptr++;
    }

    if (strncmp(http_request->recv_buffer, "GET ", 4) != 0 && strncmp(http_request->recv_buffer, "OPTIONS ", 8) != 0)
    {
        printf("HTTPServerConnection_ParseHeader: Unsupported HTTP method\n");
        return -1;
    }

    char *path_start = strchr(http_request->recv_buffer, ' ');
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

    http_request->url = strndup(path_start, path_end - path_start);
    if (http_request->url == NULL)
    {
        printf("HTTPServerConnection_ParseHeader: Failed to copy url\n");
        return -1;
    }

    return 0;
}



void HTTPRequest_Dispose(HTTPRequest* http_request)
{
    if(http_request == NULL)
    return;

    if(http_request->url != NULL)
    {
        free(http_request->url);
        http_request->url = NULL;
    }

    if(http_request->request_body != NULL)
    {
        free(http_request->request_body);
        http_request->request_body = NULL;
    }
}