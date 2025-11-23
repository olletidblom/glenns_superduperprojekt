#include "API.h"
#include "Parser.h"
#include "CURL.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
int API_ParseRequest(void* _Context);

int API_Initiate(API* api, WeatherServerInstance* instance, API_OnRequest onRequest, API_SendRequest sendRequest)
{
    api->instance = instance;
    api->onRequest = onRequest;
    api->sendRequest = sendRequest;
    api->context = instance->connection;
    WeatherServerInstance_SetCallback(api->instance, api, API_ParseRequest);

    return 0;
}
*/

int API_GetWeatherData(char *end_point)
{

    char latitude[32];
    char longitude[32];

    if (Parser_FindInString(end_point, "lat=", latitude, sizeof(latitude)) != 0)
    {
        printf("API_GetWeatherData: Failed to parse latitude\n");
        return -1;
    }
    if (Parser_FindInString(end_point, "lon=", longitude, sizeof(longitude)) != 0)
    {
        printf("API_GetWeatherData: Failed to parse longitude\n");
        return -1;
    }

    printf("API_GetWeatherData: Latitude: %s, Longitude: %s\n", latitude, longitude);

    return 0;
}

int API_CityToCoordinates(API *api, char *end_point)
{
    char city[128];
    char country[8];
    char request[256];
    char* host = "http://geocoding-api.open-meteo.com";
    char* path = "/v1/search?name=";

    if (Parser_FindInString(end_point, "city=", city, sizeof(city)) != 0)
    {
        printf("API_CityToCoordinates: Failed to parse city\n");
        return -1;
    }

    if (Parser_FindInString(end_point, "countryCode=", country, sizeof(country)) != 0)
    {
        snprintf(request, sizeof(request), "%s%s%s", host, path, city);
    }
    else
    {
        snprintf(request, sizeof(request), "%s%s%s&countryCode=%s", host, path, city, country);
    }
//http://geocoding-api.open-meteo.com/v1/search?name=Stockholm&countryCode=SE
    api->instance->request = strndup(request, sizeof(request));
    return 0;
}

int API_ProcessRequest(void *_Context)
{


    API *api = (API *)calloc(1, sizeof(API));

    api->instance = (HTTPServerConnection *)_Context;

    if (api->instance == NULL)
    {
        printf("API_ParseRequest: Instance is NULL\n");
        free(api);
        return -1;
    }

    char *url = api->instance->url;

    if (url == NULL)
    {
        free(api);
        return -2;
    }
    char *ep_start = strstr(url, "v1/");

    if (ep_start != NULL)
    {
        ep_start = ep_start + 3;
    }
    else
    {
        free(api);
        return -3;
    }

    char *ep_end = strchr(ep_start, '?');

    if (ep_end != NULL)
    {

        int ep_length = ep_end - ep_start;
        char *end_point = strndup(ep_start, ep_length);
        if (end_point == NULL)
            return -4;

        printf("Endpoint %s\n", end_point);
        if (strcmp(end_point, "gwd") != 0 && strcmp(end_point, "geo") != 0 && strcmp(end_point, "gp") != 0)
        {
            printf("API: Invalid end point!\n");
            free(end_point);
            free(api);
            return -1;
        }

        if (strcmp(end_point, "gwd") == 0)
            API_GetWeatherData(ep_start);
        else if (strcmp(end_point, "geo") == 0)
            API_CityToCoordinates(api, ep_start);
        // else if(strcmp(end_point, "gp") == 0)
        // API_GetParameters(ep_start);

        printf("API: EndPoint: %s\n", end_point);
        free(end_point);
        free(api);
        return 0;
    }
    return -2;
}