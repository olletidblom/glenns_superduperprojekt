#include "HTTPServerConnection.h"
#include "HTTPServer.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>  //for INT_MAX at content-length check

// Forward declaration
void HTTPServer_RemoveConnection(void* server, HTTPServerConnection *connection);

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

int HTTPServerConnection_Write(HTTPServerConnection *connection, char *buffer, size_t length);

void HTTPServerConnection_work(void *_Context, uint64_t monTime);

void HTTPServerConnection_Disconnect(HTTPServerConnection *connection);

int HTTPServerConnection_Initialize(HTTPServerConnection **connection, int socket, void* server_context)
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
  
  _Connection->state = HTTPServerConnection_State_Read_header;
  
  _Connection->task = smw_create_task(_Connection, HTTPServerConnection_work);
  *connection = _Connection;
  return 0;
}


int HTTPServerConnection_SendResponse(HTTPServerConnection* connection, char *body)
{
  if (connection == NULL)
    return -1;

  body = "{\"name\":\"Alice\",\"age\":30}";


  char response[1024];
  int length = snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n" "Content-Length: %zu\r\n" "Content-Type: application/json\r\n" "\r\n" "%s", strlen(body), body);
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
    case HTTPServerConnection_State_Read_header:
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

        if (connection->recv_buffer_length < 18)
            return;
        
        if (strstr(connection->recv_buffer, "\r\n\r\n") == NULL)
            return;

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
            printf("HTTPServerConnection_TaskWork: Unsupported HTTP method\n");
            connection->state = HTTPServerConnection_State_Cleanup;
            return;
        }

    

        char *path_start = strchr(connection->recv_buffer, ' ');
        if (path_start == NULL)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            return;
        }
        path_start++;
 
        char *path_end = strchr(path_start, ' ');
        if (path_end == NULL)
        {
        connection->state = HTTPServerConnection_State_Cleanup;
        return;
        }

        connection->url_path = strndup(path_start, path_end - path_start);
        if (connection->url_path == NULL)
        {
            connection->state = HTTPServerConnection_State_Cleanup;
            printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
              return;
        }
        
        

        char *host = strstr(connection->recv_buffer, "Host: ");
        if (host != NULL)
        {
            host += 6; // skip function call strlen("Host: ");
            char *eol = strstr(host, "\r\n");

            if (eol != NULL)
            {
                connection->host = strndup(host, eol - host);
                if (connection->host == NULL)
                {
                    printf("HTTPServerConnection_TaskWork: Failed to copy host\n");
                    connection->state = HTTPServerConnection_State_Cleanup;
                    return;
                }
            }
        }

        char *content_length_str = strstr(connection->recv_buffer, "Content-Length: ");
        if (content_length_str != NULL)
        {
            content_length_str += 16;       // skip function call strlen("Content-Length: ");
            char *endptr;

            long val = strtol(content_length_str, &endptr, 10);

            if (endptr == content_length_str)
            {
                printf("HTTPServerConnection_TaskWork: Invalid Content-Length value\n");
                connection->state = HTTPServerConnection_State_Cleanup;
                return;
            }

            if (val < 0 || val > INT_MAX)
            {
                printf("HTTPServerConnection_TaskWork: Content-Length out of range\n");
                connection->state = HTTPServerConnection_State_Cleanup;
                return;
            }
            
            connection->content_length = (int)val;
        } else {
            connection->content_length = 0; // No content
        }


        printf("HTTP: %s %s HOST: %s Content-length: %d SOCKET: %d\n",
            connection->method_url,
            connection->url_path,
            connection->host ? connection->host : "(null)",
            connection->content_length,
            connection->socket);
    
        connection->state = HTTPServerConnection_State_Route;
    } break;

    case HTTPServerConnection_State_Route:                          //mergea med handlers?
    {

        if (connection->method_url == NULL || connection->url_path == NULL )
        {
            printf("Route: Missing method or url_path\n");
            connection->state = HTTPServerConnection_State_Cleanup;
            return;
        }

        if (connection->host != NULL)
        {
            char temp_buffer[1024];
            snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", connection->host, connection->url_path);       //http://host/path remember this
            
            connection->url = strdup(temp_buffer);
            
            if (connection->url == NULL)
            {
                printf("Route: failed to allocate URL\n");
                connection->state = HTTPServerConnection_State_Cleanup;
                return;
            }

        }
        printf("Route %s %s\n", connection->method_url, connection->url_path);
        
        connection->state = HTTPServerConnection_State_Handlers;
    } break;

    case HTTPServerConnection_State_Handlers:
    {
        if (strcmp(connection->method_url, "GET") == 0)             //strcmp istället för strncmp kanske byta
        {
            if (strcmp(connection->url_path, "/") == 0)
            {
            printf("Handling GET / path\n");
            //could set response data here
            } else {
                printf("Unhandled GET path not existing: %s\n", connection->url_path);
            }
        }    
        else if (strcmp(connection->method_url, "POST") == 0)
        {
            if (strcmp(connection->url_path, "/") == 0)
            {
                printf("Handling POST / path\n");
                //could set response data here
            } else {
                printf("Unhandled POST path not existing: %s\n", connection->url_path);
            }
        }

        connection->state = HTTPServerConnection_State_Response;
    } break;


    case HTTPServerConnection_State_Response:
    {
        HTTPServerConnection_SendResponse(connection, NULL);
        printf("Response: Sent response on socket %d\n", connection->socket);
        connection->state = HTTPServerConnection_State_Cleanup;

    } break;


    case HTTPServerConnection_State_Cleanup:
    {
        
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
        printf("Cleanup: Socket %d\n", connection->socket);
        tcpserver_disconnect(connection->socket);

        // Destroy task first to stop work function from being called again
        if (connection->task != NULL)
        {
            smw_destroy_task(connection->task);             //handle here or in server?/dispose below
            connection->task = NULL;
        }
        
        // Now notify server to dispose (task is NULL, server knows it's done)
        // Note: Don't call dispose here - it would free memory we're using!
        // The server should check for task==NULL and dispose later
    } break;
  }
}

                    //Let the work function finish the dispose
void HTTPServerConnection_Dispose(HTTPServerConnection **connection)
{
  if (connection == NULL || *connection == NULL)
    return;

  HTTPServerConnection *_server = *connection;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);

  close(_server->socket);
  free(_server);
  *connection = NULL;
}
