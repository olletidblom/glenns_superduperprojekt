#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define _POSIX_C_SOURCE 200809L

#include <stddef.h>

typedef struct
{
  char *url;

  char recv_buffer[2048];
  size_t recv_buffer_length;

  int content_length;

  
  char *request_body; 
} HTTPRequest;

//This gets called in HTTPServerConnection.c, any mallocs inside gets freed in HTTPServerConnection_Dispose

int HTTPParser_ParseHeader(HTTPRequest *request);

#endif 