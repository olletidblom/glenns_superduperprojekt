#ifndef HTTP_SERVER_HANDLER_H
#define HTTP_SERVER_HANDLER_H


#define _POSIX_C_SOURCE 200809L

typedef struct {
    char* key;
    char* value;
} HTTPInputParameters;

typedef struct{
    char* end_point;
    HTTPInputParameters* parameters;  
}HTTPServerHandler;


void HTTPServerHandler_ParseInputParameters(const char* url);


#endif // HTTP_SERVER_HANDLER_H