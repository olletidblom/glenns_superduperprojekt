#ifndef API_H
#define API_H

#include "../WeatherServerInstance.h"
#include <stdint.h>

typedef int (*API_OnRequest)(void* _Context, char* _Response);
typedef int (*API_SendRequest)(void* _Context, char* _Host, char* _Request);

typedef struct
{
    void* context;
    API_OnRequest onRequest;
    API_SendRequest sendRequest;
    WeatherServerInstance* instance;
    char* parsed_response;
} API;

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

int API_Initiate(API* api, WeatherServerInstance* instance, API_OnRequest onRequest, API_SendRequest sendRequest);



#endif 