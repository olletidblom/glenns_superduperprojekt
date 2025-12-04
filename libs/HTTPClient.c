#include "HTTPClient.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void HTTPClient_Work(void *_Context, uint64_t _MonTime)
{
	HTTPClient_s *client = (HTTPClient_s *)_Context;

	if (client == NULL)
		return;

	switch (client->state)
	{
	case HTTPClient_State_Init:
		client->state = HTTPClient_State_Connect;
		break;
	case HTTPClient_State_Connect:
		if (tcp_connect(&client->tcp_client) == 0)
		{
			client->state = HTTPClient_State_Transmit;
			break;
		}
		break;
	case HTTPClient_State_Transmit:
		ssize_t bytesSent = tcp_send(&client->tcp_client, client->bufferPtr, client->length);
		if (bytesSent > 0)
		{
			

			//	client->state = HTTPClient_State_Receive;
			client->bufferPtr += bytesSent;
			client->length -= bytesSent;
		}

		if (client->length <= 0)
		{
			client->state = HTTPClient_State_Receive;
			memset(client->buffer, 0, 4096);
			client->bufferPtr = client->buffer;
		}
		break;
	case HTTPClient_State_Receive:
		size_t temp = 4096 - (client->bufferPtr - client->buffer);

		ssize_t bytesRead = tcp_recieve(&client->tcp_client, client->bufferPtr, temp);
		printf("in work function %d\n", temp);
		if (bytesRead > 0)
		{
			client->bufferPtr += bytesRead;
			printf("response so far: %s\n", client->buffer);
			
		}
		
		if (bytesRead == 0)
		{
			*client->bufferPtr = '\0';
			client->state = HTTPClient_State_Close;
		}

		if (bytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}
			client->state = HTTPClient_State_Close;
		}

		break;
	case HTTPClient_State_Close:
		tcp_disconnect(&client->tcp_client);
		break;
	}
}

int HTTPClient_Initiate(HTTPClient_s *_Client)
{

	_Client = (HTTPClient_s *)malloc(sizeof(HTTPClient_s));
	memset(_Client, 0, sizeof(HTTPClient_s));

	_Client->buffer = NULL;
	_Client->task = NULL;
	_Client->state = HTTPClient_State_Init;
	return 0;
}

int HTTPClient_GET(HTTPClient_s *_Client, const char *_URL, const char* host, void (*callback)(HTTPClient_s *_CLient, const char *_Event))
{

	tcp_init(&_Client->tcp_client, host, 10180);

	_Client->buffer = malloc(4096);
	if (_Client->buffer == NULL)
		return -1;

	_Client->length = snprintf(_Client->buffer, 4096, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: MyClient/1.0\r\nConnection: close\r\n\r\n", _URL, host);

	_Client->bufferPtr = _Client->buffer;
	_Client->state = HTTPClient_State_Connect;
	_Client->task = smw_create_task(_Client, HTTPClient_Work);
}

void HTTPClient_Dispose_s(HTTPClient_s *_Client)
{
	if (_Client->task != NULL)
		smw_destroy_task(_Client->task);

	if (_Client->buffer != NULL)
		free(_Client->buffer);

	if (_Client != NULL)
		free(_Client);
}
