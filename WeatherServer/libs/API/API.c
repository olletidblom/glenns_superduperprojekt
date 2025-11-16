#include "API.h"
#include "Parser.h"
#include <stdlib.h>
#include <stdio.h>

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


int API_GetWeatherData(char* end_point)
{
    
    char latitude[32];
    char longitude[32];

    if(Parser_FindInString(end_point, "lat=", latitude, sizeof(latitude)) != 0)
    {
        printf("API_GetWeatherData: Failed to parse latitude\n");
        return -1;
    }
    if(Parser_FindInString(end_point, "lon=", longitude, sizeof(longitude)) != 0)
    {
        printf("API_GetWeatherData: Failed to parse longitude\n");
        return -1;
    }

    printf("API_GetWeatherData: Latitude: %s, Longitude: %s\n", latitude, longitude);

    return 0;
}

int API_CityToCoordinates(API* api, char* end_point)
{
    char city[128];
    char country[8];
    char request[256];
    Geocoding_API* geo = &api->geocoding;
    geo->host = "http://geocoding-api.open-meteo.com";
    geo->path = "/v1/search?name=";

    if(Parser_FindInString(end_point, "city=", city, sizeof(city)) != 0)
    {
        printf("API_CityToCoordinates: Failed to parse city\n");
        return -1;
    }


    if(Parser_FindInString(end_point, "countryCode=", country, sizeof(country)) != 0)
    {
        snprintf(request, sizeof(request), "%s%s%s", geo->host, geo->path, city);
    }
    else
    {
        snprintf(request, sizeof(request), "%s%s%s&countryCode=%s", geo->host, geo->path, city, country);
    }
    
    
    CurlResponse* cresp = (CurlResponse*)api->sendRequest(api->context, request, Curl_HTTPGet);
    printf("API_CityToCoordinates: Response Data: %s\n", cresp->data);
    api->parsed_response = cresp->data;
    return 0;
}


int API_ParseRequest(void* _Context)
{
    API* api = (API*)_Context;
    //WeatherServerInstance* instance = api->instance;


    if(api->instance == NULL)
    {
        printf("API_ParseRequest: Instance is NULL\n");
        return -1;
    }

    char* url = api->instance->connection->url;

    char* ep_start = strstr(url, "v1/");

    if(ep_start != NULL)
    {
        ep_start = ep_start + 3;
    }


    char* ep_end = strchr(ep_start, '?');
    
    if(ep_end != NULL)
    {
        int ep_length = ep_end - ep_start;
        char* end_point = strndup(ep_start, ep_length);

        if(strcmp(end_point, "gwd") != 0 && strcmp(end_point, "geo") != 0 && strcmp(end_point, "gp") != 0)
        {
            printf("API: Invalid end point!\n");
            return -1;
        }

        if(strcmp(end_point, "gwd") == 0)
        API_GetWeatherData(ep_start);
        else if(strcmp(end_point, "geo") == 0)
        API_CityToCoordinates(api, ep_start);
        //else if(strcmp(end_point, "gp") == 0)
        //API_GetParameters(ep_start);


        printf("API: EndPoint: %s\n", end_point);
    }
    api->onRequest(api->context, api->parsed_response);
    return 0;
}