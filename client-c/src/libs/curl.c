#include "curl.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>




size_t Curl_WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t real_size = size * nmemb;
    CurlResponse* response = (CurlResponse*)userp;

    char* ptr = (char*)realloc(response->data, response->size + real_size + 1);
    if(ptr == NULL)
    {
        return 0;
    }

    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, real_size);
    response->size += real_size;
    response->data[response->size] = '\0';

    return real_size;
}

CurlResponse* Curl_HTTPGet(const char* url)
{
    CURL* curl;
    CURLcode res;
    CurlResponse* response = (CurlResponse*)malloc(sizeof(CurlResponse));
    if(response == NULL)
    {
        return NULL;
    }
    response->data = (char*)malloc(1);
    response->size = 0;

    curl = curl_easy_init();
    if(!curl)
    {
        free(response->data);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)response);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        fprintf(stderr, "Curl_HTTPGet: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(response->data);
        response->data = NULL;
        free(response);
        response = NULL;
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return response;
}

void Curl_Dispose(CurlResponse** _ResponsePtr)
{
    if(_ResponsePtr == NULL || *(_ResponsePtr) == NULL)
        return;

    CurlResponse* response = *(_ResponsePtr);
    if(response->data != NULL)
        free(response->data);

    free(response);
    *(_ResponsePtr) = NULL;
}