#ifndef __HANDLER_H__
#define __HANDLER_H__

#define _POSIX_C_SOURCE 200809L

#include "../HTTPServerConnection.h"

// Now we can use the full type
typedef char* (*RequestHandler)(void* _Context);

typedef struct {
    char* path;
    char* method;
    RequestHandler handler;
} Route;


void HTTPServer_RegisterRoute(const char *end_point, RequestHandler handler);


RequestHandler HTTPServer_FindRoute(const char *end_point);

void HTTPServer_RouteCleanup();

#endif // __HANDLER_H__