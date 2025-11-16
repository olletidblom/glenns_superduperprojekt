#ifndef API_H
#define API_H

#include "../WeatherServerInstance.h"
#include "../CURL.h"
#include <stdint.h>

typedef int (*API_OnRequest)(void* _Context, char* _Response);
typedef void* (*API_SendRequest)(void* _Context, char* _Request, void* (*_OnResponse)(char* _Response));


typedef struct
{
    const char* host;
}Weather_API;

typedef struct
{
    const char* host;
    const char* path;
    const char* city_name;
}Geocoding_API;

typedef struct
{
    void* context;
    API_OnRequest onRequest;
    API_SendRequest sendRequest;
    WeatherServerInstance* instance;
    Geocoding_API geocoding;
    Weather_API weather;
    char* parsed_response;
} API;



int API_Initiate(API* api, WeatherServerInstance* instance, API_OnRequest onRequest, API_SendRequest sendRequest);



#endif 