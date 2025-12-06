#ifndef Meteo_API_H
#define Meteo_API_H

#define _POSIX_C_SOURCE 200809L
#include "../glenns_metro/libs/jansson/jansson.h"
#include "../server/src/Handlers/HTTPServerHandler.h"
#include "HTTPClient.h"
#include "curl.h"

typedef struct
{
    char* latitude;
    char* longitude;
    char *result;
    char* url;
    char* file_path;
    CurlResponse* response;
    HTTPClient_s http_client;
} Meteo_API;

int Meteo_Init(Meteo_API **_MeteoApiPtr, const char* latitude, const char* longitude);

int Meteo_SendRequest(Meteo_API *Meteo_api, HTTPServerHandler* handler);

void Meteo_Dispose(Meteo_API **_MeteoApiPtr);

#endif // Meteo_API_H