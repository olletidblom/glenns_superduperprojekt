#ifndef API_H
#define API_H
#define _POSIX_C_SOURCE 200809L
#include "../HTTPServerConnection.h"
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
    HTTPServerConnection* instance;
    Geocoding_API geocoding;
    Weather_API weather;
    char* parsed_response;
} API;





int API_ProcessRequest(void* _Context);


#endif 