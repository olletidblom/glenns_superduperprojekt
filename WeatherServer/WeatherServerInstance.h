
#ifndef __WeatherServerInstance_h_
#define __WeatherServerInstance_h_

#include "smw.h"
#include "HTTPServer/HTTPServerConnection.h"

typedef int (*WeatherServerInstance_CB)(void* _Context);


typedef struct
{
	void* context;
	HTTPServerConnection* connection;
	WeatherServerInstance_CB onRequest;
} WeatherServerInstance;


int WeatherServerInstance_Initiate(WeatherServerInstance* _Instance, HTTPServerConnection* _Connection);
int WeatherServerInstance_InitiatePtr(HTTPServerConnection* _Connection, WeatherServerInstance** _InstancePtr);

void WeatherServerInstance_SetCallback(WeatherServerInstance* _Instance, void* _Context, WeatherServerInstance_CB _OnRequest);

void WeatherServerInstance_Work(WeatherServerInstance* _Instance, uint64_t _MonTime);

void WeatherServerInstance_Dispose(WeatherServerInstance* _Instance);
void WeatherServerInstance_DisposePtr(WeatherServerInstance** _InstancePtr);

#endif //__WeatherServerInstance_h_
