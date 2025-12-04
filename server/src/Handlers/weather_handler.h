#ifndef WEATHER_HANDLER_H
#define WEATHER_HANDLER_H

#include "HTTPServerHandler.h"

char* Handle_Weather(void* _Context);
char* Handle_GEO(void* _Context);
char* Handle_UsersPOST(void* _Context);

#endif