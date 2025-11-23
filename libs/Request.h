#ifndef REQUEST_H
#define REQUEST_H

#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include "HTTPServerConnection.h"



char *Request_HandleRequest(uint8_t *buffer, size_t length);

int Request_ServerHandleRequest(void* _Context);

#endif