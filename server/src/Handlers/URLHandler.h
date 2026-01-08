#ifndef HTTP_SERVER_HANDLER_H
#define HTTP_SERVER_HANDLER_H

#define _POSIX_C_SOURCE 200809L

typedef char *(*RouteFunction)(void *_Context);

typedef struct
{
    char *key;
    char *value;
} InputParameters;

typedef struct
{
    int pairsLength;
    int maxPairs;
    char *end_point;
    InputParameters *parameters;
} URLHandler;

int URLHandler_Initialize(URLHandler **_ServerHandler);

int URLHandler_Parse(URLHandler *_URLHandler, char *url);

char *URLHandler_GetParameterValue(URLHandler *_ServerHandler, const char *key);

void URLHandler_Dispose(URLHandler **_ServerHandler);

#endif // HTTP_SERVER_HANDLER_H