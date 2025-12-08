
#include "handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ROUTES 10

static Route routes[MAX_ROUTES];
static size_t route_count = 0;

void HTTPServer_RegisterRoute(const char *end_point, RequestHandler handler)
{
    if (route_count >= MAX_ROUTES)
        return;

    Route *r = &routes[route_count++];
    r->path = strdup(end_point);
    r->method = NULL;
    r->handler = handler;
    return;
}

RequestHandler HTTPServer_FindRoute(const char *end_point)
{
    for (size_t i = 0; i < route_count; i++)
    {

        Route *r = &routes[i]; 


        if (strcmp(r->path, end_point) == 0)
        {
            RequestHandler temp = r->handler;
            return r->handler;
        }
    }

    return NULL;
}

void HTTPServer_RouteCleanup()
{
    for(size_t i = 0; i < route_count; i++)
    {
        if(routes[i].path != NULL)
        free(routes[i].path);
    }
}