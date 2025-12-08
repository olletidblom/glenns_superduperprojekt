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

#define RESPONSE_HEADER "HTTP/1.1 %d %s\r\n"                             \
                        "Content-Length: %zu\r\n"                        \
                        "Content-Type: application/json\r\n"             \
                        "Access-Control-Allow-Origin: *\r\n"             \
                        "Access-Control-Allow-Methods: GET, OPTIONS\r\n" \
                        "Access-Control-Allow-Headers: Content-Type\r\n" \
                        "\r\n"                                           \
                        "%s"

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Dispose(HTTPServerConnection **server);

// Every connection gets its own HTTPServerConnection instance
int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, int *is_active)
{

    if (connection == NULL)
        return -1;

    HTTPServerConnection *_Connection = (HTTPServerConnection *)calloc(1, sizeof(HTTPServerConnection));

    _Connection->http_request = (HTTPRequest){0};
    _Connection->http_response = (HTTPResponse){0};
    _Connection->socket = socket;
    _Connection->is_active = is_active;

    _Connection->http_response.status_code = 200;

    _Connection->state = HTTPServerConnection_State_Read;
    _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);
    *connection = _Connection;
    return 0;
}

int HTTPConnection_Read(HTTPServerConnection *connection, uint8_t *buffer, size_t length)
{
    return recv(connection->socket, buffer, length, MSG_DONTWAIT);
}

int HTTPConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length)
{
    ssize_t result = send(connection->socket, buffer, length, MSG_NOSIGNAL);
    return result;
}

int HTTPServerConnection_Read(HTTPServerConnection *connection)
{
    int bytesRead = HTTPConnection_Read(connection, (uint8_t *)(connection->http_request.recv_buffer + connection->http_request.recv_buffer_length),
                                        sizeof(connection->http_request.recv_buffer) - connection->http_request.recv_buffer_length - 1);

    if (bytesRead < 0)
    {
        return -1;
    }

    if (bytesRead == 0)
    {
        connection->state = HTTPServerConnection_State_Cleanup;
        return -2;
    }
    connection->http_request.recv_buffer_length += bytesRead;
    connection->http_request.recv_buffer[connection->http_request.recv_buffer_length] = '\0';

    if (connection->http_request.url != NULL && connection->http_request.recv_buffer_length < 18)
        return -3;

    return 0;
}

// Gets called inside HTTPServerHandler_Initialize
void HTTPServerConnection_SetCallback(void *_Connection, void *_Context, OnParse onHandle)
{
    HTTPServerConnection *connection = (HTTPServerConnection *)_Connection;
    if (connection == NULL)
        return;

    connection->context = _Context;
    connection->handler_parse = onHandle;
}

int HTTPServerConnection_SendResponse(HTTPServerConnection *connection, char *body)
{
    if (connection == NULL)
        return -1;

    if (connection->http_response.response_body != NULL)
    {
        body = connection->http_response.response_body;
    }
    else
    {
        body = "{\"message\":\"No response\"}";
    }

    const char *status_text;
    switch (connection->http_response.status_code)
    {
    case 200:
        status_text = "OK";
        break;
    case 404:
        status_text = "Not Found";
        break;
    case 500:
        status_text = "Internal Server Error";
        break;
    default:
        status_text = "Unknown";
        break;
    }

    char response[2048];
    int length = snprintf(response, sizeof(response), RESPONSE_HEADER,
                          connection->http_response.status_code, status_text, strlen(body), body);

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
    case HTTPServerConnection_State_Read:
    {
        if (HTTPServerConnection_Read(connection) == 0)
        {
            connection->state = HTTPServerConnection_State_Parse;
            break;
        }
        else
            return;
    }
    break;
    case HTTPServerConnection_State_Parse:
    {
        if (HTTPParser_ParseHeader(&connection->http_request) == 0)
        {
            connection->state = HTTPServerConnection_State_FindRoute;
            break;
        }
        else
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            break;
        }
    }
    break;

    case HTTPServerConnection_State_FindRoute:
    {
        // handler_parse returns the function pointer to the correct handler based on the endpoint from URL
        connection->handler_process = connection->handler_parse(connection->context, connection->http_request.url);

        if (connection->handler_process == NULL)
        {
            printf("No handler found: %s\n", connection->http_request.url);
            connection->http_response.status_code = 404;
            connection->state = HTTPServerConnection_State_Response;
            break;
        }
        connection->state = HTTPServerConnection_State_Handlers;
    }
    break;

    case HTTPServerConnection_State_Handlers:
    {
        connection->http_response.response_body = connection->handler_process(connection->context);
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
        HTTPServerConnection_Dispose(&connection);
    }
    break;
    }
}

//Anything malloced that is returned or created inside HTTPServerConnection_work gets freed here
void HTTPServerConnection_Dispose(HTTPServerConnection **connection)
{
    if (connection == NULL || *connection == NULL)
        return;

    HTTPServerConnection *_server = *connection;

    if (_server->task != NULL)
    {
        smw_destroy_task(_server->task);
        _server->task = NULL;
    }
    if (_server->http_request.url != NULL)
    {
        free(_server->http_request.url);
        _server->http_request.url = NULL;
    }
    if (_server->http_request.request_body != NULL)
    {
        free(_server->http_request.request_body);
        _server->http_request.request_body = NULL;
    }
    if (_server->http_response.response_body != NULL)
    {
        free(_server->http_response.response_body);
        _server->http_response.response_body = NULL;
    }

    tcpserver_disconnect(_server->socket);

    // HTTPClient_Dispose_s(&_server->http_client);
    *(_server->is_active) = 1;
    free(_server);
    _server = NULL;
}
