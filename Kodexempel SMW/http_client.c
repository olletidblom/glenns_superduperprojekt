#include "http_client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//---------------Internal functions----------------

void http_client_work(void* _Context, uint64_t _MonTime);
void http_client_dispose(http_client** _ClientPtr);

//----------------------------------------------------

int http_client_init(const char* _URL, http_client** _ClientPtr)
{
	if(_URL == NULL || _ClientPtr == NULL)
		return -1;

	if(strlen(_URL) > http_client_max_url_length)
		return -2;

	http_client* _Client = (http_client*)malloc(sizeof(http_client));
	if(_Client == NULL)
		return -3;

	_Client->state = http_client_state_init;
	_Client->task = smw_createTask(_Client, http_client_work);

	_Client->callback = NULL;
	_Client->timer = 0;
	_Client->isConnected = 0;

	strcpy(_Client->url, _URL);

	*(_ClientPtr) = _Client;

	return 0;
}

int http_client_get(const char* _URL, uint64_t _Timeout, void (*_Callback)(const char* _Event, const char* _Response))
{
	http_client* client = NULL;
	if(http_client_init(_URL, &client) != 0)
		return -1;

	client->timeout = _Timeout;
	client->callback = _Callback;

	return 0;
}

http_client_state http_client_work_init(http_client* _Client)
{
	return http_client_state_connect;
}

http_client_state http_client_work_connect(http_client* _Client)
{
	return http_client_state_connecting;
}

http_client_state http_client_work_connecting(http_client* _Client)
{
	if(_Client->isConnected)
		return http_client_state_writing;

	return http_client_state_connecting;
}

http_client_state http_client_work_writing(http_client* _Client)
{
	return http_client_state_reading;
}

http_client_state http_client_work_reading(http_client* _Client)
{	
	return http_client_state_done;
}

http_client_state http_client_work_done(http_client* _Client)
{
	if(_Client->callback != NULL)
		_Client->callback("RESPONSE", "Det här är responsen!");

	return http_client_state_dispose;
}

void http_client_work(void* _Context, uint64_t _MonTime)
{
	http_client* _Client = (http_client*)_Context;

	if(_Client->timer == 0)
	{
		_Client->timer = _MonTime;
	}
	else if(_MonTime >= _Client->timer + _Client->timeout)
	{
		if(_Client->callback != NULL)
			_Client->callback("TIMEOUT", NULL);

		http_client_dispose(&_Client);
		return;
	}

	printf("%i > %s\r\n", _Client->state, _Client->url);

	switch(_Client->state)
	{
		case http_client_state_init:
		{
			_Client->state = http_client_work_init(_Client);
		} break;
		
		case http_client_state_connect:
		{
			_Client->state = http_client_work_connect(_Client);
		} break;
		
		case http_client_state_connecting:
		{
			_Client->state = http_client_work_connecting(_Client);
		} break;
		
		case http_client_state_writing:
		{
			_Client->state = http_client_work_writing(_Client);
		} break;
		
		case http_client_state_reading:
		{
			_Client->state = http_client_work_reading(_Client);
		} break;
		
		case http_client_state_done:
		{
			_Client->state = http_client_work_done(_Client);
		} break;
		
		case http_client_state_dispose:
		{
			http_client_dispose(&_Client);
		} break;
		
	}
	
}

void http_client_dispose(http_client** _ClientPtr)
{
	if(_ClientPtr == NULL || *(_ClientPtr) == NULL)
		return;

	http_client* _Client = *(_ClientPtr);

	if(_Client->task != NULL)
		smw_destroyTask(_Client->task);

	free(_Client);

	*(_ClientPtr) = NULL;
}
