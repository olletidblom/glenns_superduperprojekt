#include "CURL.h"
#include <stdlib.h>
#include <string.h>



size_t Curl_WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t real_size = size * nmemb;
    CurlResponse* response = (CurlResponse*)userp;

    char* ptr = realloc(response->data, response->size + real_size + 1);
    if(!ptr)
        return 0; // out of memory

    response->data = ptr;
    memcpy(response->data + response->size, contents, real_size);
    response->size += real_size;
    response->data[response->size] = '\0';

    return real_size;
}

void* Curl_HTTPGet(const char* url)
{
    if(!url) return NULL;

    CurlResponse* response = calloc(1, sizeof(CurlResponse));
    if(!response) return NULL;

    response->data = malloc(1);
    if(!response->data) {
        free(response);
        return NULL;
    }
    response->data[0] = '\0';

    CURL* curl = curl_easy_init();
    if(!curl) {
        free(response->data);
        free(response);
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
        free(response->data);
        free(response);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return response;
}

int Curl_HTTPGetServer(void* _Context)
{
    HTTPServerConnection* connection = (HTTPServerConnection*)_Context;

    if(connection == NULL)
    return -1;
    
    CurlResponse* response = Curl_HTTPGet(connection->request);

    if(response == NULL)
    return -2;
    
    connection->response = strndup(response->data, response->size);
    connection->response_size = response->size;

    free(response->data);
    free(response);

    ///printf("resdsa: %s\n", response->data);
    return 0;
}