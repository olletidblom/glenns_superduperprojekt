#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H


#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
typedef struct{
  int status_code;
  char* response_ptr;
  char* response;
  char* response_formatted;
  int response_length;
}HTTPResponse;




int HTTPResponse_Initialize(HTTPResponse* response);

int HTTPResponse_Format(HTTPResponse* response);

int HTTPResponse_Send(int socket, HTTPResponse* response);

void HTTPResponse_Dispose(HTTPResponse* response);

#endif