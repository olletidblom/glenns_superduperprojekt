#include "WeatherHandler.h"
#include "../../glenns_metro/src/City.h"
#include "../../libs/API/GEO_API.h"
#include "../../libs/API/Meteo_API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *Handle_Weather(void *_Context)
{
    URLHandler* url_handler = (URLHandler*)_Context;
    char* lat_temp = URLHandler_GetParameterValue(url_handler, "lat");
    char* lon_temp = URLHandler_GetParameterValue(url_handler, "lon");
    if (lat_temp == NULL || lon_temp == NULL)
    {
        return strdup("{\"error\":\"No lat or lon parameter\"}");
    }

    Meteo_API *geo = NULL;

    Meteo_Init(&geo, lat_temp, lon_temp);
    
    int res = Meteo_SendRequest(geo, url_handler);

    if (res != 0)
    {
        printf("Meteo_SendRequest failed with code: %d\n", res);
        Meteo_Dispose(&geo);
        return strdup("{\"error\":\"Failed to get weather data\"}");
    }

    char *result = strdup(geo->parsed_response);

    Meteo_Dispose(&geo);

    return result;
}

char *Handle_GEO(void *_Context)
{

    URLHandler* url_handler = (URLHandler*)_Context;

    char* city_name = URLHandler_GetParameterValue(url_handler, "city");
    if (city_name == NULL)
    {
        return strdup("{\"error\":\"No city parameter\"}");
    }

    GEO_API *geo = NULL;

    GEO_Init(&geo, city_name);
    
    int res = GEO_SendRequest(geo);

    if (res != 0)
    {
        printf("GEO_SendRequest failed with code: %d\n", res);
        GEO_Dispose(&geo);

        return strdup("{\"error\":\"Failed to get GEO data\"}");
    }

    char *result = strdup(geo->parsed_response);
    GEO_Dispose(&geo);

    return result;
}
