#ifndef CURL_H
#define CURL_H


#define _POSIX_C_SOURCE 200809L

#include <curl/curl.h>
#include <string.h>
#include "HTTPServerConnection.h"

typedef struct {
    char* data;
    size_t size;
} CurlResponse;

int Curl_HTTPGetServer(void* _Context);
void* Curl_HTTPGet(const char* url);


#endif 