#ifndef __HANDLER_H__
#define __HANDLER_H__

#define _POSIX_C_SOURCE 200809L



typedef char* (*RouteFunction)(void* _Context);

typedef struct {
    char* path;
    char* method;
    RouteFunction handler;
} Route;


void RouteHandler_Init(const char *end_point, RouteFunction handler);


RouteFunction RouteHandler_FindRoute(const char *end_point);

void RouteHandler_Dispose();

#endif 