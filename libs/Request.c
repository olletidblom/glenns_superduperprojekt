#include "Request.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

char *Request_HandleRequest(uint8_t *buffer, size_t length)
{
  if (buffer == NULL)
  {
    return NULL;
  }

  char temp_buffer[8192];
  char *method_temp = NULL;
  char *path_temp = NULL;
  char *host_temp = NULL;

  char *ptr = (char *)buffer;

  char *method = strchr(ptr, ' ');

  if (method != NULL)
  {
    int method_length = method - ptr;

    method_temp = strndup(ptr, method_length);
    if (method_temp == NULL)
    {
      printf("HTTPServerConnection_TaskWork: Failed to copy method\n");
      return NULL;
    }
  }

  if ((strcmp(method_temp, "POST") != 0) && (strcmp(method_temp, "GET") != 0))
  {
    return NULL;
  }

  char *url_path = strchr(ptr, '/');

  if (url_path != NULL)
  {
    char *url_end = strchr(url_path, ' ');

    if (url_end != NULL)
    {
      int url_length = url_end - url_path;

      path_temp = strndup(url_path, url_length);
      if (path_temp == NULL)
      {
        printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
        free(method_temp);
        return NULL;
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

      host_temp = strndup(host, url_length);
      if (host_temp == NULL)
      {
        printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
        free(method_temp);
        free(path_temp);
        return NULL;
      }
    }
  }

  if (!method_temp || !host_temp || !path_temp)
    return NULL;

  int result = snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", host_temp, path_temp);

  if (result > 0)
  {
    free(method_temp);
    free(path_temp);
    free(host_temp);
    return strdup(temp_buffer);
  }

  free(method_temp);
  free(path_temp);
  free(host_temp);

  return NULL;
}

int Request_ServerHandleRequest(void* _Context)
{
  HTTPServerConnection* connection = (HTTPServerConnection*)_Context;
  if(connection->buffer == NULL)
  printf("FUNCTION CALLED!\n");


  printf("FUNCTION CALLED!\n");
  connection->buffer[connection->buffer_length] = '\0';
  printf("BUFFER: %s", connection->buffer);
  if (strstr(connection->buffer, "\r\n\r\n"))
  {
    connection->url = Request_HandleRequest(connection->buffer, connection->buffer_length);
    printf("%s\n", connection->url);
  }

  if (connection->url == NULL)
    return -1;

  return 0;
}