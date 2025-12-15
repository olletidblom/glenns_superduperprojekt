#ifndef WEATHER_HANDLER_H
#define WEATHER_HANDLER_H

#define _POSIX_C_SOURCE 200809L

#include "../HTTPServerConnection.h"

char* Handle_Weather(void* _Context);
char* Handle_GEO(void* _Context);

#endif