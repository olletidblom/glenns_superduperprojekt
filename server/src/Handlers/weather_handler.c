#include "weather_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* Handle_Weather(void* _Context)
{
    HTTPServerHandler* handler = (HTTPServerHandler*)_Context;
    printf("Weather handler called %s\n", handler->parameters->value);
    char lat_temp[32];
    char lon_temp[32];
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
    }
    printf("Latitude: %s, Longitude: %s\n", lat_temp, lon_temp);
    HTTPServerHandler_Dispose(&handler);
    return strdup("{\"temperature\":25,\"condition\":\"sunny\",\"location\":\"Stockholm\"}");
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