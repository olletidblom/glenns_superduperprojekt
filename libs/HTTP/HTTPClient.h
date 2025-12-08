#ifndef HTTPClient_h
#define HTTPClient_h

#include "TCPClient.h"
#include "smw.h"

typedef enum
{
	HTTPClient_State_Init,
	HTTPClient_State_Connect,
	HTTPClient_State_Transmit,
	HTTPClient_State_Receive,
	HTTPClient_State_Close

} HTTPClient_State;

typedef struct HTTPClient_s HTTPClient_s;

struct HTTPClient_s
{
	void (*callback)(HTTPClient_s* _CLient, const char* _Event);
	uint8_t* buffer;
	uint8_t* bufferPtr;
	uint8_t* response_buffer;
	int length;
	TCPClient tcp_client;
	smw_task* task;
	HTTPClient_State state;
};



int HTTPClient_Initiate(HTTPClient_s* _Client);

int HTTPClient_GET(HTTPClient_s* _Client, const char* _URL, const char* host, void (*callback)(HTTPClient_s* _CLient, const char* _Event));

void HTTPClient_Dispose_s(HTTPClient_s* _Client);



#endif //HTTPClient_h