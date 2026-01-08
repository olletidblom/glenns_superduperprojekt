
#include "RouteHandler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ROUTES 10

static Route routes[MAX_ROUTES];
static size_t route_count = 0;

void RouteHandler_Init(const char *end_point, RouteFunction handler)
{
    if (route_count >= MAX_ROUTES)
        return;

    Route *r = &routes[route_count++];
    r->path = strdup(end_point);
    r->method = NULL;
    r->handler = handler;
    return;
}

RouteFunction RouteHandler_FindRoute(const char *end_point)
{
    for (size_t i = 0; i < route_count; i++)
    {

        Route *r = &routes[i]; 


        if (strcmp(r->path, end_point) == 0)
        {
            return r->handler;
        }
    }

    return NULL;
}

void RouteHandler_Dispose()
{
    for(size_t i = 0; i < route_count; i++)
    {
        if(routes[i].path != NULL)
        free(routes[i].path);
    }
}