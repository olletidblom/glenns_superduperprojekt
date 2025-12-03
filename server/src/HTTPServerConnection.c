#include "HTTPServerConnection.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h> //for INT_MAX at content-length check

// Type alias for handler function pointer
typedef char *(*HandlerFunc)(HTTPServerConnection *);

// Forward declaration
void HTTPServer_RemoveConnection(void *server, HTTPServerConnection *connection);

int HTTPConnection_Read(HTTPServerConnection *connection, uint8_t *buffer, size_t length);
int HTTPServerConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length);

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Cleanup(HTTPServerConnection *connection);
void HTTPServerConnection_Disconnect(HTTPServerConnection *connection);

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, void *server_context, int *is_active)
{

    if (connection == NULL)
        return -1;

    HTTPServerConnection *_Connection = (HTTPServerConnection *)calloc(1, sizeof(HTTPServerConnection));

    _Connection->socket = socket;
    _Connection->is_active = is_active;

    _Connection->status_code = 200;

    _Connection->state = HTTPServerConnection_State_Read_Make_URL;
    _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);
    *connection = _Connection;
    return 0;
}

int HTTPConnection_Read(HTTPServerConnection *connection, uint8_t *buffer, size_t length)
{
    // printf("send error %d: %s\n", errno, strerror(errno));
    return recv(connection->socket, buffer, length, MSG_DONTWAIT);
}

int HTTPConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length)
{
    ssize_t result = send(connection->socket, buffer, length, MSG_NOSIGNAL);
    // printf("send error %d: %s\n", errno, strerror(errno));
    // scanf("%d", result);
    return result;
}

int HTTPServerConnection_ParseHeader(HTTPServerConnection *connection)              // Returns: 0 on success, -1 on error, 1 if needs more data
{
    if (connection == NULL)
        return -1;

    char *header_end = strstr(connection->recv_buffer, "\r\n\r\n");
    if (header_end == NULL)
    {
    return 1; 
    }

    size_t header_size = (header_end + 4) - connection->recv_buffer;
    if (header_size > 1024)
    {
        printf("HTTPServerConnection_ParseHeader: Headers too large (buffer full)\n");
        return -1;
    }

    char *check_ptr = connection->recv_buffer;
    while (check_ptr < connection->recv_buffer + connection->recv_buffer_length)
    {
        if (*check_ptr == '\n')
        {
            if (check_ptr == connection->recv_buffer || *(check_ptr - 1) != '\r')
            {
                printf("HTTPServerConnection_ParseHeader: Invalid line ending (bare LF without CR)\n");
                return -1;
            }
        }
        check_ptr++;
    }

    if (strncmp(connection->recv_buffer, "GET ", 4) != 0 && strncmp(connection->recv_buffer, "OPTIONS ", 8) != 0)
    {
        printf("HTTPServerConnection_ParseHeader: Unsupported HTTP method\n");
        return -1;
    }

    char *path_start = strchr(connection->recv_buffer, ' ');
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

    connection->url = strndup(path_start, path_end - path_start);
    if (connection->url == NULL)
    {
        printf("HTTPServerConnection_ParseHeader: Failed to copy url\n");
        return -1;
    }

    return 0;
}

void HTTPServerConnection_SetCallback(void* _Connection, void *_Context, OnParse onHandle)
{
    HTTPServerConnection* connection = (HTTPServerConnection*)_Connection;
    if (connection == NULL)
        return;

    printf("pointer: %p\n", connection);
    connection->context = _Context;
    connection->handler_parse = onHandle;
}

int HTTPServerConnection_SendResponse(HTTPServerConnection *connection, char *body)
{
    if (connection == NULL)
        return -1;

    if (connection->response_body != NULL)
    {
        body = connection->response_body;
    }
    else if (connection->request_body != NULL)
    {
        body = connection->request_body;
    }
    else
    {
        body = "{\"message\":\"No response\"}";
    }

    const char *status_text;
    switch (connection->status_code)
    {
    case 200:
        status_text = "200 OK";
        break;
    case 404:
        status_text = "404 Not Found";
        break;
    case 500:
        status_text = "500 Internal Server Error";
        break;
    default:
        status_text = "Unknown";
        break;
    }

    char response[2048];
    int length = snprintf(response, sizeof(response),
                          "HTTP/1.1 %d %s\r\n"
                          "Content-Length: %zu\r\n"
                          "Content-Type: application/json\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                          "Access-Control-Allow-Headers: Content-Type\r\n"
                          "\r\n"
                          "%s",
                          connection->status_code, status_text, strlen(body), body);

    int result = HTTPConnection_Write(connection, response, length);
    return result;
}

void HTTPServerConnection_work(void *_Context, uint64_t monTime)
{
    HTTPServerConnection *connection = (HTTPServerConnection *)_Context;
    if (connection == NULL)
        return;

    switch (connection->state)
    {
    case HTTPServerConnection_State_Read_Make_URL:
    {
        int bytesRead = HTTPConnection_Read(connection, (uint8_t *)(connection->recv_buffer + connection->recv_buffer_length),
                                            sizeof(connection->recv_buffer) - connection->recv_buffer_length - 1);

        if (bytesRead < 0)
        {
            return;
        }

        if (bytesRead == 0)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            return;
        }
        connection->recv_buffer_length += bytesRead;
        connection->recv_buffer[connection->recv_buffer_length] = '\0';

        
        if (connection->url == NULL && connection->recv_buffer_length >= 18)
        {
            int parse_result = HTTPServerConnection_ParseHeader(connection);
            
            if (parse_result == 1) 
            {
                return;
            }
            
            if (parse_result < 0) 
            {
                printf("Failed to parse HTTP request (error %d)\n", parse_result);
                connection->state = HTTPServerConnection_State_Cleanup;
                return;
            }
        }

        connection->handler_process = connection->handler_parse(connection->context, connection->url);

        if (connection->handler_process == NULL)
        {
            printf("No handler found: %s\n", connection->url);
            connection->status_code = 404;
            connection->state = HTTPServerConnection_State_Response;
            break;
        }
        
        connection->state = HTTPServerConnection_State_Handlers;
    }
    break;

    case HTTPServerConnection_State_Handlers:
    {
        printf("are we crashing here?\n");
        connection->response_body = connection->handler_process(connection->context);
        printf("response!!! %s \n", connection->response_body);
        connection->state = HTTPServerConnection_State_Response;
    }
    break;

    case HTTPServerConnection_State_Response:
    {
        HTTPServerConnection_SendResponse(connection, NULL);
        printf("Response: Sent response on socket %d\n", connection->socket);
        connection->state = HTTPServerConnection_State_Cleanup;
    }
    break;

    case HTTPServerConnection_State_Cleanup:
    {
        printf("cleaned up!\n");
        HTTPServerConnection_Cleanup(connection);
    }
    break;
    }
}

void HTTPServerConnection_Cleanup(HTTPServerConnection *connection)
{

    if (connection == NULL)
        return;

    if (connection->url != NULL)
    {
        free(connection->url);
        connection->url = NULL;
    }
    if (connection->request_body != NULL)
    {
        free(connection->request_body);
        connection->request_body = NULL;
    }

    free(connection->response_body);

    printf("Cleanup: Socket %d\n", connection->socket);
    tcpserver_disconnect(connection->socket);

    HTTPServerConnection_Dispose(&connection);
    *(connection->is_active) = 1;
}


void HTTPServerConnection_Dispose(HTTPServerConnection **connection)
{
    if (connection == NULL || *connection == NULL)
        return;

    HTTPServerConnection *_server = *connection;

    if (_server->task != NULL)
        smw_destroy_task(_server->task);

    free(_server);
}
