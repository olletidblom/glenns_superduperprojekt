#ifndef WEATHER_HANDLER_H
#define WEATHER_HANDLER_H

#include "handler.h"

char* Handle_Weather(HTTPServerConnection* conn);
char* Handle_UsersGET(HTTPServerConnection* conn);
char* Handle_UsersPOST(HTTPServerConnection* conn);

#endif