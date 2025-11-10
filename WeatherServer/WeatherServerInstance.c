#include "WeatherServerInstance.h"
#include <stdlib.h>

//-----------------Internal Functions-----------------

int WeatherServerInstance_OnRequest(void* _Context);

//----------------------------------------------------

int WeatherServerInstance_Initiate(WeatherServerInstance* _Instance, HTTPServerConnection* _Connection)
{
	_Instance->connection = _Connection;

	HTTPServerConnection_SetCallback(_Instance->connection, _Instance, WeatherServerInstance_OnRequest);

	return 0;
}

int WeatherServerInstance_InitiatePtr(HTTPServerConnection* _Connection, WeatherServerInstance** _InstancePtr)
{
	if(_InstancePtr == NULL)
		return -1;

	WeatherServerInstance* _Instance = (WeatherServerInstance*)malloc(sizeof(WeatherServerInstance));
	if(_Instance == NULL)
		return -2;

	int result = WeatherServerInstance_Initiate(_Instance, _Connection);
	if(result != 0)
	{
		free(_Instance);
		return result;
	}

	*(_InstancePtr) = _Instance;

	return 0;
}

int WeatherServerInstance_OnRequest(void* _Context)
{
	WeatherServerInstance* _Instance = (WeatherServerInstance*)_Context;

	if(_Instance == NULL || _Instance->connection == NULL || _Instance->connection->url == NULL)
	return -1;




	
	printf("Endpoint: %s\n", _Instance->connection->httpRequest.end_point);
	_Instance->onRequest(_Instance->context);
	return 0;
}


void WeatherServerInstance_SetCallback(WeatherServerInstance* _Instance, void* _Context, WeatherServerInstance_CB _OnRequest)
{
	_Instance->context = _Context;
	_Instance->onRequest = _OnRequest;
}


void WeatherServerInstance_Work(WeatherServerInstance* _Server, uint64_t _MonTime)
{
	
}

void WeatherServerInstance_Dispose(WeatherServerInstance* _Instance)
{

}

void WeatherServerInstance_DisposePtr(WeatherServerInstance** _InstancePtr)
{
	if(_InstancePtr == NULL || *(_InstancePtr) == NULL)
		return;

	WeatherServerInstance_Dispose(*(_InstancePtr));
	free(*(_InstancePtr));
	*(_InstancePtr) = NULL;
}
