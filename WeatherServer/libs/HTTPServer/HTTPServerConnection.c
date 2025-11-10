#include "HTTPServerConnection.h"
#include <stdlib.h>
#include <stdio.h>

//-----------------Internal Functions-----------------

void HTTPServerConnection_TaskWork(void *_Context, uint64_t _MonTime);

//----------------------------------------------------

int HTTPServerConnection_Initiate(HTTPServerConnection *_Connection, int _FD)
{
	TCPClient_Initiate(&_Connection->tcpClient, _FD);

	_Connection->task = smw_createTask(_Connection, HTTPServerConnection_TaskWork);

	return 0;
}

int HTTPServerConnection_InitiatePtr(int _FD, HTTPServerConnection **_ConnectionPtr)
{
	if (_ConnectionPtr == NULL)
		return -1;

	HTTPServerConnection *_Connection = (HTTPServerConnection *)malloc(sizeof(HTTPServerConnection));
	if (_Connection == NULL)
		return -2;

	int result = HTTPServerConnection_Initiate(_Connection, _FD);
	if (result != 0)
	{
		free(_Connection);
		return result;
	}

	*(_ConnectionPtr) = _Connection;

	return 0;
}

void HTTPServerConnection_SetCallback(HTTPServerConnection *_Connection, void *_Context, HTTPServerConnection_OnRequest _OnRequest)
{
	_Connection->context = _Context;
	_Connection->onRequest = _OnRequest;
}

int HTTPServerConnection_SendRequest(void *_Context, char *_Host, char *_Request)
{

	return 0;
}

int HTTPServerConnection_SendResponse(void *_Context, char *_Response)
{
	// HTTPServerConnection* _Connection = (HTTPServerConnection*)_Context;
	_Response = "HELLO CLIENT!!";
	printf("Sending Response: %s\n", _Response);
	// TCPClient_Write(&_Connection->tcpClient, (const uint8_t*)_Response, strlen(_Response));
	return 0;
}

void HTTPServerConnection_TaskWork(void *_Context, uint64_t _MonTime)
{
	HTTPServerConnection *_Connection = (HTTPServerConnection *)_Context;

	if (_Connection == NULL)
		return;

	_Connection->method = NULL;
	_Connection->url = NULL;
	_Connection->host = NULL;
	_Connection->url_path = NULL;

	char buffer[1024];

	int bytesRead = TCPClient_Read(&_Connection->tcpClient, (uint8_t *)buffer, sizeof(buffer));


	if (bytesRead > 0)
	{
		// printf("Data: %s\n", buffer);
		buffer[bytesRead] = '\0';
		char *ptr = &buffer[0];
		char *method = strchr(ptr, ' ');
		if (method != NULL)
		{
			int method_length = method - ptr;

			_Connection->method = strndup(ptr, method_length);

			if (_Connection->method == NULL)
			{
				printf("HTTPServerConnection_TaskWork: Failed to copy method\n");
			}
		}

		char *url_path = strchr(ptr, '/');

		if (url_path != NULL)
		{
			char *url_end = strchr(url_path, ' ');

			if (url_end != NULL)
			{
				int url_length = url_end - url_path;

				_Connection->url_path = strndup(url_path, url_length);
				if (_Connection->url_path == NULL)
				{
					printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
				}
			}
		}

		char *host = strstr(ptr, "Host: ");
		if (host != NULL)
		{
			host += strlen("Host: ");

			char *eol = strstr(host, "\r\n");

			if (eol != NULL)
			{
				int url_length = eol - host;

				_Connection->host = strndup(host, url_length);

				if (_Connection->host == NULL)
				{
					printf("HTTPServerConnection_TaskWork: Failed to copy url\n");
				}
			}
		}
	}

	if (_Connection->method != NULL && _Connection->host != NULL && (strcmp(_Connection->method, "GET") == 0))
	{
		char temp_buffer[1024];

		snprintf(temp_buffer, sizeof(temp_buffer), "http://%s%s", _Connection->host, _Connection->url_path);
		// printf("Temp URL: %s\n", temp_buffer);
		free(_Connection->url_path);
		free(_Connection->host);
		free(_Connection->method);

		_Connection->url = strdup(temp_buffer);
		if (strstr(buffer, "\r\n\r\n"))
			_Connection->onRequest(_Connection->context);
	}

	/*if(bytesRead > 0)
	{

		buffer[bytesRead] = '\0';
		char* ptr = &buffer[0];

		if(ptr == NULL)
		{
			return;
		}
		char* line = strndup(ptr, *strchr(buffer, ' '));

		char* pch = strtok(line, " ");
		_Connection->method = strdup(pch);

		pch = strtok(NULL, "\r\n");
		_Connection->url =  strdup(pch);
	}
	else
	{
		return;
	}	*/
}

void HTTPServerConnection_Dispose(HTTPServerConnection *_Connection)
{
	TCPClient_Dispose(&_Connection->tcpClient);
	smw_destroyTask(_Connection->task);
}

void HTTPServerConnection_DisposePtr(HTTPServerConnection **_ConnectionPtr)
{
	if (_ConnectionPtr == NULL || *(_ConnectionPtr) == NULL)
		return;

	HTTPServerConnection_Dispose(*(_ConnectionPtr));
	free(*(_ConnectionPtr));
	*(_ConnectionPtr) = NULL;
}
