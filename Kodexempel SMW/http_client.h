#ifndef __http_client_h_
#define __http_client_h_

#include "smw.h"

#ifndef http_client_max_url_length
	#define http_client_max_url_length 1024
#endif

typedef enum
{
	http_client_state_init = 0,
	http_client_state_connect = 1,
	http_client_state_connecting = 2,
	http_client_state_writing = 3,
	http_client_state_reading = 4,
	http_client_state_done = 5,
	http_client_state_dispose = 6

} http_client_state;

typedef struct
{
	http_client_state state;
	smw_task* task;
	char url[http_client_max_url_length + 1];
	uint64_t timeout;
	
	void (*callback)(const char* _Event, const char* _Response);

	uint64_t timer;

	int isConnected;

} http_client;


int http_client_get(const char* _URL, uint64_t _Timeout, void (*_Callback)(const char* _Event, const char* _Response));


#endif //__http_client_h_