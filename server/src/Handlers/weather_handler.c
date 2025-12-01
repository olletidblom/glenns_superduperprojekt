#include "weather_handler.h"
#include "../../glenns_metro/src/City.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* Handle_Weather(void* _Context)
{
    HTTPServerHandler* handler = (HTTPServerHandler*)_Context;
    printf("Weather handler called %s\n", handler->parameters->value);
    char lat_temp[32];
    char lon_temp[32];
    char city_name[64];
    char country_code[8];
    for(int i = 0; i < handler->parameters->pairsLength; i++)
    {
        if(strcmp(handler->parameters[i].key, "lat") == 0)
        {
            snprintf(lat_temp, sizeof(lat_temp), "%s", handler->parameters[i].value);
            lat_temp[sizeof(lat_temp)-1] = '\0';
        }
        else if(strcmp(handler->parameters[i].key, "lon") == 0)
        {
            snprintf(lon_temp, sizeof(lon_temp), "%s", handler->parameters[i].value);
            lon_temp[sizeof(lon_temp)-1] = '\0';
        }

        if(strcmp(handler->parameters[i].key, "city") == 0)
        {
            snprintf(city_name, sizeof(city_name), "%s", handler->parameters[i].value);
            city_name[sizeof(city_name)-1] = '\0';
        }
        else if(strcmp(handler->parameters[i].key, "countryCode") == 0)
        {
            snprintf(country_code, sizeof(country_code), "%s", handler->parameters[i].value);
            country_code[sizeof(country_code)-1] = '\0';
        }
    }
    City* city = NULL;
    City_Init(city_name, country_code, 0, 0, &city);
    if(city == NULL)
    {
        printf("Failed to create City object\n");
    }
    char* weather_data = City_GetAll(city);
    HTTPServerHandler_Dispose(&handler);
    City_Dispose(&city);
    return strdup(weather_data);
}

char* Handle_UsersGET(void* _Context)
{
    printf("Users GET handler called\n");
    return strdup("{\"users\":[{\"id\":1,\"name\":\"Alice\"},{\"id\":2,\"name\":\"Bob\"}]}");
}

char* Handle_UsersPOST(void* _Context)
{
    HTTPServerHandler* handler = (HTTPServerHandler*)_Context;
    printf("Users POST handler called - received: %s\n", handler->parameters->value) ;
    // Here you would normally process conn->request_body
    return strdup("{\"status\":\"User created successfully\"}");
}