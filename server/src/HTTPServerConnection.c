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

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, void *server_context)
{

    if (connection == NULL)
        return -1;

    HTTPServerConnection *_Connection = (HTTPServerConnection *)malloc(sizeof(HTTPServerConnection));

    _Connection->socket = socket;
    _Connection->method_url = NULL;
    _Connection->url = NULL;
    _Connection->host = NULL;
    _Connection->url_path = NULL;
    _Connection->context = server_context;

    _Connection->content_length = 0;
    _Connection->recv_buffer_length = 0;
    _Connection->recv_buffer[0] = '\0';

    _Connection->request_body = NULL;

    _Connection->handler = NULL;
    _Connection->response_body = NULL;
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

// Returns: 0 on success, -1 on error, 1 if needs more data
int HTTPServerConnection_ParseHeader(HTTPServerConnection *connection)
{
    if (connection == NULL)
        return -1;

    if (connection->recv_buffer_length < 18)
        return 1; // Need more data

    if (connection->recv_buffer_length > 1024)
    {
        printf("HTTPServerConnection_ParseHeader: Headers too large (>1024 bytes)\n");
        return -1;
    }

    if (strstr(connection->recv_buffer, "\r\n\r\n") == NULL) // if headers not complete
        return 1;                                            // Need more data

    // Validate HTTP line endings - reject requests with bare LF
    char *check_ptr = connection->recv_buffer;
    while (*check_ptr != '\0' && check_ptr < connection->recv_buffer + connection->recv_buffer_length)
    {
        if (*check_ptr == '\n')
        {
            // Check if preceded by \r
            if (check_ptr == connection->recv_buffer || *(check_ptr - 1) != '\r')
            {
                printf("HTTPServerConnection_ParseHeader: Invalid line ending (bare LF without CR)\n");
                return -1;
            }
        }
        check_ptr++;
    }

    // Parse method
    if (strncmp(connection->recv_buffer, "GET ", 4) == 0)
    {
        connection->method_url = strndup("GET", 3);
    }
    else if (strncmp(connection->recv_buffer, "POST ", 5) == 0)
    {
        connection->method_url = strndup("POST", 4);
    }
    else
    {
        printf("HTTPServerConnection_ParseHeader: Unsupported HTTP method\n");
        return -1;
    }

    // Parse path
    char *path_start = strchr(connection->recv_buffer, ' ');
    if (path_start == NULL)
        return -1;

    path_start++;
    char *path_end = strchr(path_start, ' ');
    if (path_end == NULL)
        return -1;

    connection->url_path = strndup(path_start, path_end - path_start);
    if (connection->url_path == NULL)
    {
        free(connection->method_url);
        printf("HTTPServerConnection_ParseHeader: Failed to copy url\n");
        return -1;
    }

    // Parse Host header
    char *host = strstr(connection->recv_buffer, "Host: ");
    if (host != NULL)
    {
        host += 6;
        char *eol = strstr(host, "\r\n");
        if (eol != NULL)
        {
            connection->host = strndup(host, eol - host);
            if (connection->host == NULL)
            {
                free(connection->url_path);
                free(connection->method_url);
                printf("HTTPServerConnection_ParseHeader: Failed to copy host\n");
                return -1;
            }
        }
    }

    if (connection->host != NULL && connection->url_path != NULL && connection->method_url != NULL)
    {
        char temp_buffer[2048];
        snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", connection->host, connection->url_path);
        connection->url = strdup(temp_buffer);
        printf("Built URL: %s\n", connection->url);
        return 0;
    }

    free(connection->method_url);
    free(connection->url_path);
    return -1;
}

void HTTPServerConnection_SetCallback(HTTPServerConnection* connection, void* _Context, void (*callback)(HTTPServerConnection* _Connection))
{
    if (connection == NULL || callback == NULL)
        return;

    connection->context = _Context;
    connection->handler = (void*)callback;
    
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

    char response[1024];
    int length = snprintf(response, sizeof(response),
                          "HTTP/1.1 %d %s\r\n"
                          "Content-Length: %zu\r\n"
                          "Content-Type: application/json\r\n"
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

        // Try to parse the header (only once)
        if (connection->method_url == NULL)
        {
            int parse_result = HTTPServerConnection_ParseHeader(connection);
            if (parse_result == 1)
            {
                return; // Need more header data
            }
            else if (parse_result < 0)
            {
                connection->state = HTTPServerConnection_State_Cleanup;
                return;
            }
        }



       // HTTPServerHandler_ParseInputParameters(connection->url);
        // Check if we need to read body (happens every time we read more data)

        // No body expected - build URL and move to handlers

        //HTTPServer_RegisterRoute(connection, "GET", "/", Handle_Weather);
        //HTTPServer_RegisterRoute(connection, "GET", "/api/v1/gwd?lat=5.1232&lon=5.32132", Handle_Weather);
        //HTTPServer_RegisterRoute(connection, "GET", "/users", Handle_UsersGET);
        //HTTPServer_RegisterRoute(connection, "POST", "/users", Handle_UsersPOST);

        connection->handler = HTTPServer_FindHandler(connection,
                                                     connection->method_url,
                                                     connection->url_path);

        if (connection->handler == NULL)
        {
            printf("No handler found: %s %s\n", connection->method_url, connection->url_path);
            connection->status_code = 404;
        }

        connection->state = HTTPServerConnection_State_Handlers;
    }
    break;

    case HTTPServerConnection_State_Handlers:
    {
        if (connection->handler != NULL)
        {
            HandlerFunc handler = (HandlerFunc)connection->handler;
            connection->response_body = handler(connection);
            printf("Handler executed for %s %s\n", connection->method_url, connection->url_path);
        }
        else
        {
            connection->status_code = 404;
            connection->response_body = strdup("{\"error\":\"Not Found\"}");
        }

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
        HTTPServerConnection_Cleanup(connection);
    }
    break;
    }
}



void HTTPServerConnection_Cleanup(HTTPServerConnection *connection)
{
    if (connection == NULL)
        return;

    if (connection->url_path != NULL)
    {
        free(connection->url_path);
        connection->url_path = NULL;
    }
    if (connection->host != NULL)
    {
        free(connection->host);
        connection->host = NULL;
    }
    if (connection->method_url != NULL)
    {
        free(connection->method_url);
        connection->method_url = NULL;
    }
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

    // Destroy task to stop work function from being called again
    if (connection->task != NULL)
    {
        smw_destroy_task(connection->task);
        connection->task = NULL;
    }
}

// Let the work function finish the dispose
void HTTPServerConnection_Dispose(HTTPServerConnection **connection)
{
    if (connection == NULL || *connection == NULL)
        return;

    HTTPServerConnection *_server = *connection;

    if (_server->task != NULL)
        smw_destroy_task(_server->task);

    close(_server->socket);
    free(_server->host);
    free(_server->method_url);
    free(_server->url_path);
    free(_server->url);
    free(_server);
    *connection = NULL;
}
