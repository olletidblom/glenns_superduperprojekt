#ifndef CURL_H
#define CURL_H




#include <curl/curl.h>
#include <string.h>

typedef struct {
    char* data;
    size_t size;
} CurlResponse;

size_t Curl_WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void* Curl_HTTPGet(char* url);



#endif 