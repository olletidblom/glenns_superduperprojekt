
#include "handler.h"
#include <string.h>

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
    printf("route crash1234 %s\n", r->path);
    return;
}

RequestHandler HTTPServer_FindRoute(const char *end_point)
{
    printf("route crash1 %s\n", end_point);
    for (size_t i = 0; i < route_count; i++)
    {

        Route *r = &routes[i]; 


        printf("route crash2 \n");
        if (strcmp(r->path, end_point) == 0)
        {
            printf("i=%zu route_count=%zu r=%p\n", i, route_count, r); //gpt focus here!!!
            RequestHandler temp = r->handler;
            printf("fndksgnfkog: %p\n", temp);
            return r->handler;
        }
    }

    return NULL;
}

void HTTPServer_RouteCleanup()
{
    for(int i = 0; i < route_count; i++)
    {
        if(routes[i].path != NULL)
        free(routes[i].path);
    }
}