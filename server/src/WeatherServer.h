#ifndef WEATHERSERVER_H
#define WEATHERSERVER_H

#include "HTTPServer.h"

typedef struct
{
    HTTPServer* http_server;
    Cities* cities;
} WeatherServer;


int WeatherServer_Initialize(WeatherServer** server);

void WeatherServer_Dispose(WeatherServer** server);

#endif