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

void HTTPServer_OnConnect(void* _Context, int socket);
int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length);

void HTTPServer_work(void *_Context, uint64_t monTime);

void HTTPServer_RemoveConnection(HTTPServer *server, HTTPServerConnection *connection);
void HTTPServer_Disconnect(HTTPServer *server);


int HTTPServer_Initialize(HTTP_Method method, HTTPServer **server)
{

  HTTP_Status status;
  TCPServer* tcp_server = (TCPServer *)malloc(sizeof(TCPServer));
  HTTPServer *http_server = (HTTPServer *)malloc(sizeof(HTTPServer));

  http_server->task = smw_create_task(http_server, HTTPServer_work);
  http_server->status = http_server_initialized;
  http_server->method = method;


  //initialize routes
  http_server->route_count = 0;
  http_server->max_routes = 10;
  http_server->routes = (Route *)malloc(sizeof(Route) * http_server->max_routes);  //array of Route structs

  printf("Initializing TCP server\n");
  tcpserver_listen(tcp_server, 10180, 1000, HTTPServer_OnConnect, http_server);

  *server = http_server;
  return 0;
}



void HTTPServer_OnConnect(void* _Context, int socket)
{
  printf("HTTP Server: Connection received on socket %d\n", socket);
  HTTPServer* server = (HTTPServer *)_Context;

  if (server == NULL)
    return;

  HTTPServerConnection* connection = NULL;
  if( HTTPServerConnection_Initialize(&connection, socket, server) < 0) // added , server
  {
    printf("Failed to initialize HTTP server connection\n");
    return;
  }

}

int HTTPServer_Write(HTTPServer *server, char *buffer, size_t length)
{
  int result = tcpserver_send(server->tcp_server, (char *)buffer, length);

  printf("message sent: %s\n", buffer);
  return result;
}

int HTTPServer_Read(HTTPServer *server, uint8_t *buffer, size_t length)
{
  int result = tcpserver_recieve(server->tcp_server, buffer, length);

  return result;
}

int HTTPServer_ParseHeader(HTTPServer *server)
{

  if (server == NULL)
    return -1;

  server->method_url = NULL;
  server->url = NULL;
  server->host = NULL;
  server->url_path = NULL;

  char buffer[1024];

  int bytesRead = HTTPServer_Read(server, (uint8_t *)buffer, sizeof(buffer));

  if (bytesRead < 0)
  {
    return -1;
  }

  if (bytesRead > 0)
  {
    // printf("Data: %s\n", buffer);
    buffer[bytesRead] = '\0';
    char *ptr = &buffer[0];
    char *method = strchr(ptr, ' ');
    if (method != NULL)
    {
      int method_length = method - ptr;

      server->method_url = strndup(ptr, method_length);

      if (server->method_url == NULL)
      {
        printf("HTTPServerConnection_TaskWork: Failed to copy method\n");
      }
    }

    char *url_path = strchr(ptr, '/');

    if (url_path != NULL)
    {
      char *url_end = strchr(url_path, ' ');

      if (url_end != NULL)
      {
        int url_length = url_end - url_path;

        server->url_path = strndup(url_path, url_length);
        if (server->url_path == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
        }
      }
    }

    char *host = strstr(ptr, "Host: ");
    if (host != NULL)
    {
      host += strlen("Host: ");

      char *eol = strstr(host, "\r\n");

      if (eol != NULL)
      {
        int url_length = eol - host;

        server->host = strndup(host, url_length);

        if (server->host == NULL)
        {
          printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
        }
      }
    }
  }

  if (server->method_url != NULL && server->host != NULL && (strcmp(server->method_url, "GET") == 0))
  {
    char temp_buffer[1024];

    snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", server->host, server->url_path);
    // printf("Temp URL: %s\n", temp_buffer);
    free(server->url_path);
    free(server->host);
    free(server->method_url);

    server->url = strdup(temp_buffer);
    if (strstr(buffer, "\r\n\r\n"))
      printf("HTTP Server: Finished parsing header: %s\n", server->url);

  }
  return 0;
}

int HTTPServer_SendResponse(HTTPServer *server, char *body)
{
  if (server == NULL)
    return -1;

  body = "{\"name\":\"Alice\",\"age\":30}";


  char response[1024];
  int length = snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n" "Content-Length: %zu\r\n" "Content-Type: application/json\r\n" "\r\n" "%s", strlen(body), body);
  int result = HTTPServer_Write(server, response, length);
  return result;
}

void HTTPServer_RegisterRoute(HTTPServer* server, const char* method, const char* path, RequestHandler handler)
{
    if (server == NULL || method == NULL || path == NULL || handler == NULL)
        return;

    if (server->route_count >= server->max_routes)
    {
        server->max_routes *= 2;
        server->routes = realloc(server->routes, sizeof(Route) * server->max_routes);
    }

    Route* route = &server->routes[server->route_count];
    route->method = strdup(method);
    route->path = strdup(path);
    route->handler = handler;
    server->route_count++;

    printf("Registered: %s %s (total:%zu/%zu)", method, path, server->route_count, server->max_routes);
}

RequestHandler HTTPServer_FindHandler(HTTPServer* server, const char* method, const char* path)
{
    if (server ==NULL || method == NULL || path == NULL)
        return NULL;

    for (size_t i = 0; i < server->route_count; i++)
    {
        if (strcmp(server->routes[i].method, method) == 0 && strcmp(server->routes[i].path, path) == 0)
        {
            printf("found handler for %s %s\n", method, path);
            return server->routes[i].handler;
        }
    }
    printf("No handler found for %s %s\n", method, path);
    return NULL;  // fix to set status code to 404
}


void HTTPServer_work(void *_Context, uint64_t monTime)
{
  HTTPServer *server = (HTTPServer *)_Context;

  if (server == NULL)
    return;


}

void HTTPServer_RemoveConnection(HTTPServer *server, HTTPServerConnection *connection)
{
    if (server == NULL || connection == NULL)
    return;

    printf("HTTPServer: Removing connection on socket %d\n", connection->socket);

    HTTPServerConnection_Dispose(&connection);
}


void HTTPServer_Disconnect(HTTPServer *server)
{
  if (server == NULL)
    return;

  //tcpserver_disconnect(server->tcp_server, server->tcp_server->client->client_socket);
}

void HTTPServer_Dispose(HTTPServer **server)
{
  if (server == NULL || *server == NULL)
    return;

  HTTPServer *_server = *server;

  if (_server->task != NULL)
    smw_destroy_task(_server->task);

  tcpserver_dispose(_server->tcp_server);
  free(_server->url);
  free(_server);
  *server = NULL;
}
