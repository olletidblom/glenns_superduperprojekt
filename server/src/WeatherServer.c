#include "WeatherServer.h"
#include "handlers/WeatherHandler.h"
#include "handlers/RouteHandler.h"
#include <stdlib.h>

int WeatherServer_Initialize(WeatherServer **server)
{

    WeatherServer *weather_server = (WeatherServer *)malloc(sizeof(WeatherServer));

    if (weather_server == NULL)
        return -1;

    RouteHandler_Init("geo", Handle_GEO);
    RouteHandler_Init("gwd", Handle_Weather);

    Cities_Init(&weather_server->cities);

    int result = HTTPServer_Initialize(0, &weather_server->http_server);

    if (result < 0)
        return -2;

    *server = weather_server;
    return 0;
}

void WeatherServer_Dispose(WeatherServer **server)
{
    if (server == NULL || *server == NULL)
    {
        return;
    }

    WeatherServer *srv = *server;
    RouteHandler_Dispose();
    Cities_Dispose(&srv->cities);
    HTTPServer_Dispose(&srv->http_server);
    free(srv);
    *server = NULL;
}