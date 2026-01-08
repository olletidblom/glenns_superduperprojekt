#include "HTTPServerConnection_fuzz.h"
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h> //for INT_MAX at content-length check

int HTTPRequest_Initialize(HTTPRequest *http_request)
{
    if (http_request == NULL)
        return -1;

    http_request->content_length = 0;
    http_request->recv_buffer[0] = '\0';
    http_request->url = NULL;
    http_request->request_body = NULL;
    http_request->recv_buffer_length = 0;

    return 0;
}

int HTTPRequest_ParseHeader(HTTPRequest *http_request)
{
    if (http_request == NULL)
        return -1;

    char *header_end = strstr(http_request->recv_buffer, "\r\n\r\n");
    if (header_end == NULL)
    {
        return 1;
    }

    size_t header_size = (header_end + 4) - http_request->recv_buffer;
    if (header_size > 1024)
    {
        printf("HTTPServerConnection_ParseHeader: Headers too large (buffer full)\n");
        return -1;
    }

    char *check_ptr = http_request->recv_buffer;
    while (check_ptr < http_request->recv_buffer + http_request->recv_buffer_length)
    {
        if (*check_ptr == '\n')
        {
            if (check_ptr == http_request->recv_buffer || *(check_ptr - 1) != '\r')
            {
                printf("HTTPServerConnection_ParseHeader: Invalid line ending (bare LF without CR)\n");
                return -1;
            }
        }
        check_ptr++;
    }

    if (strncmp(http_request->recv_buffer, "GET ", 4) != 0 && strncmp(http_request->recv_buffer, "OPTIONS ", 8) != 0)
    {
        printf("HTTPServerConnection_ParseHeader: Unsupported HTTP method\n");
        return -1;
    }

    char *path_start = strchr(http_request->recv_buffer, ' ');
    if (path_start == NULL)
    {
        return -1;
    }

    path_start++;
    char *path_end = strchr(path_start, ' ');
    if (path_end == NULL)
    {
        return -1;
    }

    http_request->url = strndup(path_start, path_end - path_start);
    if (http_request->url == NULL)
    {
        printf("HTTPServerConnection_ParseHeader: Failed to copy url\n");
        return -1;
    }
    http_request->url_length = (size_t)(path_end - path_start);
    http_request->url[http_request->url_length] = '\0';

    return 0;
}

int URLHandler_Initialize(URLHandler **_URLHandler)
{

    URLHandler *handler = (URLHandler *)malloc(sizeof(URLHandler));

    if (handler == NULL)
        return -1;

    handler->parameters = (InputParameters *)malloc(2 * sizeof(InputParameters));

    if (handler->parameters == NULL)
        return -2;

    handler->pairsLength = 0;
    handler->maxPairs = 2;
    handler->end_point = NULL;

    *_URLHandler = handler;
    return 0;
}

int URLHandler_ParseEndPoint(URLHandler *_URLHandler, char *url)
{
    if (_URLHandler == NULL || url == NULL)
        return -1;

    char *ep_start = strstr(url, "v1/");

    if (ep_start == NULL)
        return -2;

    ep_start = ep_start + 3;

    char *ep_end = strchr(ep_start, '?');

    if (ep_end == NULL)
        return -3;

    int ep_length = ep_end - ep_start;
    _URLHandler->end_point = strndup(ep_start, ep_length);

    if (_URLHandler->end_point == NULL)
        return -4;

    return 0;
}

int URLHandler_ParseInputParameters(URLHandler *_URLHandler, char *url)
{
    int pairsLength = 0;
    int maxPairs = 2;

    char *url_cpy = NULL;

    if (_URLHandler == NULL || url == NULL)
        return -1;

    url_cpy = strndup(url, strlen(url));

    char *param_start = strchr(url_cpy, '?');

    if (param_start == NULL)
        return -2;

    param_start++;

    char *first_param = strtok(param_start, "&");
    while (first_param != NULL)
    {
        char *split = strchr(first_param, '=');
        if (split)
        {
            if (pairsLength == maxPairs)
            {
                maxPairs *= 2;
                InputParameters *temp = (InputParameters *)realloc(_URLHandler->parameters, maxPairs * sizeof(InputParameters));
                if (temp == NULL)
                {
                    free(url_cpy);
                    return -3;
                }

                _URLHandler->parameters = temp;
            }

            _URLHandler->parameters[pairsLength].key = strndup(first_param, split - first_param);
            _URLHandler->parameters[pairsLength].value = strdup(split + 1);
            pairsLength++;
            _URLHandler->pairsLength = pairsLength;
        }
        first_param = strtok(NULL, "&");
    }
    if (url_cpy != NULL)
        free(url_cpy);

    return 0;
}

char *URLHandler_GetParameterValue(URLHandler *_URLHandler, const char *key)
{
    if (_URLHandler == NULL || key == NULL)
        return NULL;

    for (int i = 0; i < _URLHandler->pairsLength; i++)
    {
        if (strcmp(_URLHandler->parameters[i].key, key) == 0)
        {
            return _URLHandler->parameters[i].value;
        }
    }

    return NULL;
}

int URLHandler_Parse(URLHandler *_URLHandler, char *url)
{

    int result = URLHandler_ParseEndPoint(_URLHandler, url);

    if (result != 0)
    {
        return -1;
    }

    result = URLHandler_ParseInputParameters(_URLHandler, url);

    if (result != 0)
    {
        return -2;
    }

    printf("Parsed endpoint: %s\n", _URLHandler->end_point);

    return 0;
}

void URLHandler_Dispose(URLHandler **_URLHandler)
{
    if (_URLHandler == NULL || *_URLHandler == NULL)
        return;

    URLHandler *handler = *_URLHandler;

    if (handler->end_point != NULL)
        free(handler->end_point);

    for (int i = 0; i < handler->pairsLength; i++)
    {
        if (handler->parameters[i].key != NULL)
            free(handler->parameters[i].key);

        if (handler->parameters[i].value != NULL)
            free(handler->parameters[i].value);
    }

    free(handler->parameters);
    free(handler);
}