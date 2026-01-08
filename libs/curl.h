#ifndef CURL_H
#define CURL_H

#include <stddef.h>

typedef struct{
    char *data;
    size_t size;
}CurlResponse;


CurlResponse* Curl_HTTPGet(char* url);

void Curl_Dispose(CurlResponse** _ResponsePtr);

#endif // CURL_H