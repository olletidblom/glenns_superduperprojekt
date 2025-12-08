#ifndef GEO_API_H
#define GEO_API_H

#define _POSIX_C_SOURCE 200809L
#include "../../glenns_metro/libs/jansson/jansson.h"
#include "../HTTP/HTTPClient.h"
#include "../curl.h"

typedef struct
{
    char *city_name;
    char *result;
    char* url;
    char* file_path;
    CurlResponse* response;
    HTTPClient_s http_client;
} GEO_API;

int GEO_Init(GEO_API **_GeoApiPtr, const char *city_name);

int GEO_CheckCache(GEO_API *geo_api);

int GEO_SendRequest(GEO_API *geo_api);

void GEO_Dispose(GEO_API **_GeoApiPtr);

#endif // GEO_API_H