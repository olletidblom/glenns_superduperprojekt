#ifndef HTTP_SERVER_HANDLER_H
#define HTTP_SERVER_HANDLER_H

#define _POSIX_C_SOURCE 200809L

#include "../HTTPServerConnection.h"

typedef char* (*RouteFunction)(void* _Context);

typedef RouteFunction (*Handler)(const char* end_point); 

typedef struct
{
    char *key;
    char *value;
    int pairsLength;
    int maxPairs;
} HTTPInputParameters;

typedef struct
{
    char *end_point;
    HTTPInputParameters *parameters;
    Handler routeHandler;
} HTTPServerHandler;

int HTTPServerHandler_Initialize(HTTPServerHandler **_ServerHandler, void* _Context, Handler _RouteHandler);

//Parses the endpoint and input parameters from the URL, returns the corresponding RouteFunction
RouteFunction HTTPServerHandler_run(void* _Context, char *url);

void HTTPServerHandler_Dispose(HTTPServerHandler **_ServerHandler);



#endif // HTTP_SERVER_HANDLER_H