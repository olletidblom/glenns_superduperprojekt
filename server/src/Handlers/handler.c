
#include "handler.h"

#define MAX_ROUTES 10

static Route *routes[MAX_ROUTES];
static size_t route_count = 0;

void HTTPServer_RegisterRoute(const char *end_point, RequestHandler handler)
{
    if (route_count >= MAX_ROUTES)
        return;

    Route *r = &routes[route_count++];

    r->path = strdup(end_point);
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
            return r->handler;
        }
    }

    return NULL;
}

void HTTPServer_RouteHandler(void *_Context, HTTPServerConnection *connection)
{
}