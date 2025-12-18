#include "HTTPServerConnection.h"
#include "Handlers/RouteHandler.h"
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

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Dispose(HTTPServerConnection **server);

// Every connection gets its own HTTPServerConnection instance
int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, int *is_active)
{

    if (connection == NULL)
        return -1;

    HTTPServerConnection *_Connection = (HTTPServerConnection *)calloc(1, sizeof(HTTPServerConnection));

    URLHandler_Initialize(&_Connection->url_handler);

    HTTPResponse_Initialize(&_Connection->http_response);

    HTTPRequest_Initialize(&_Connection->http_request);

    _Connection->socket = socket;
    _Connection->is_active = is_active;

    _Connection->http_response.status_code = 200;
    _Connection->timeout = 0;
    _Connection->bytesReadOut = 0;

    _Connection->state = HTTPServerConnection_State_Read;
    _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);
    *connection = _Connection;
    return 0;
}

// Gets called inside HTTPServerHandler_Initialize
void HTTPServerConnection_SetCallback(void *_Connection, void *_Context, Callback _Callback)
{
    HTTPServerConnection *connection = (HTTPServerConnection *)_Connection;
    if (connection == NULL)
        return;

    connection->context = _Context;
    connection->connection_callback = _Callback;
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

        int result = HTTPRequest_ReadHeaders(connection->socket, &connection->http_request, &connection->bytesReadOut);

        if (connection->timeout > 0)
        {
            if (monTime >= connection->timeout)
            {
                connection->state = HTTPServerConnection_State_Timeout;
            }
        }
        else
        {
            connection->timeout = monTime + 3000;
        }

        if (connection->bytesReadOut > 0)
        {
            connection->timeout = 0;
        }

        if (result == HTTPServerConnection_ReadResult_Success)
        {
            connection->state = HTTPServerConnection_State_Parse;
            break;
        }
        else if (result == HTTPServerConnection_ReadResult_Pending)
        {
            break;
        }
        else if (result == HTTPServerConnection_ReadResult_Error)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            break;
        }
    }
    break;
    case HTTPServerConnection_State_Parse:
    {
        int result = HTTPRequest_ParseHeader(&connection->http_request);
        if (result == 0)
        {
            connection->state = HTTPServerConnection_State_ParseURL;
            break;
        }
        else
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            break;
        }
    }
    break;
    case HTTPServerConnection_State_ParseURL:
    {
        int parse_result = URLHandler_Parse(connection->url_handler, connection->http_request.url);
        if (parse_result != 0)
        {
            connection->http_response.status_code = 400;
            connection->state = HTTPServerConnection_State_FormatResponse;
            break;
        }
        connection->state = HTTPServerConnection_State_FindRoute;
    }
    break;
    case HTTPServerConnection_State_FindRoute:
    {
        // RouteHandler_FindRoute returns the function pointer to the correct handler based on the endpoint from URL
        connection->handler = RouteHandler_FindRoute(connection->url_handler->end_point);
        if (connection->handler == NULL)
        {
            connection->http_response.status_code = 404;
            connection->state = HTTPServerConnection_State_FormatResponse;
            break;
        }
        connection->state = HTTPServerConnection_State_Handlers;
    }
    break;

    case HTTPServerConnection_State_Handlers:
    {

        connection->http_response.response = connection->handler(connection->url_handler);
        connection->state = HTTPServerConnection_State_FormatResponse;
    }
    break;

    case HTTPServerConnection_State_FormatResponse:
    {
        int result = HTTPResponse_Format(&connection->http_response);
        if (result != 0)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
        }
        connection->state = HTTPServerConnection_State_SendResponse;
    }
    break;

    case HTTPServerConnection_State_SendResponse:
    {
        int result = HTTPResponse_Send(connection->socket, &connection->http_response);
        if (result == 0)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            break;
        }
        else
        {
            break;
        }
    }
    break;

    case HTTPServerConnection_State_Timeout:
    {
        printf("Connection timed out!\n");
        connection->http_response.status_code = 408; // Request Timeout
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

// Anything malloced that is returned or created inside HTTPServerConnection_work gets freed here
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

    tcpserver_disconnect(_server->socket);

    HTTPRequest_Dispose(&_server->http_request);

    HTTPResponse_Dispose(&_server->http_response);

    URLHandler_Dispose(&_server->url_handler);

    *(_server->is_active) = 1;
    free(_server);
    _server = NULL;
}
