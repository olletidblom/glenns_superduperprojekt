#include "HTTPServerHandler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
AnonymFunkion* HTTPServerHandler_run = ChooseAPI_return_funktion; ->
weather_handler -> HTTPServerHandler_run();
*/

void HTTPServerHandler_Dispose(HTTPServerHandler **_ServerHandler);

RouteFunction HTTPServerHandler_run(HTTPServerHandler *_ServerHandler, char *url);

int HTTPServerHandler_Initialize(HTTPServerHandler **_ServerHandler, void* _Context, Handler _OnParse)
{
    
    HTTPServerHandler *handler = (HTTPServerHandler *)malloc(sizeof(HTTPServerHandler));

    if(handler == NULL)
    return -1;


    printf("hello1\n");
    handler->onParse = _OnParse;
    printf("hello\n");
    handler->parameters = (HTTPInputParameters *)malloc(2 * sizeof(HTTPInputParameters));

    if(handler->parameters == NULL)
    return -2;

    printf("hello2\n");
    handler->parameters->pairsLength = 0;
    handler->parameters->maxPairs = 2;
    handler->end_point = NULL;
    
    HTTPServerConnection_SetCallback(_Context, handler, HTTPServerHandler_run);

    *_ServerHandler = handler;
    return 0;
}

int HTTPServerHandler_ParseEndPoint(HTTPServerHandler *_ServerHandler, char *url)
{
    if (_ServerHandler == NULL || url == NULL)
        return -1;

    char *ep_start = strstr(url, "v1/");

    if (ep_start == NULL)
        return -2;

    ep_start = ep_start + 3;

    char *ep_end = strchr(ep_start, '?');

    if (ep_end == NULL)
        return -3;

    int ep_length = ep_end - ep_start;
    _ServerHandler->end_point = strndup(ep_start, ep_length);

    if(_ServerHandler->end_point == NULL)
    return -4;


    return 0;
}

int HTTPServerHandler_ParseInputParameters(HTTPServerHandler *_ServerHandler, char *url)
{
    int pairsLength = 0;
    int maxPairs = 2;

    char *url_cpy = NULL;

    if (_ServerHandler == NULL || url == NULL)
        return -1;

    url_cpy = strndup(url, strlen(url));

    char *param_start = strchr(url_cpy, '?');

    if (param_start == NULL)
    return -2;

    printf("Param_start = %s : url_cpy = %p : url_cpy_length = %d\n", param_start, url_cpy, strlen(url_cpy));
    param_start++;

    // TODO: Add error checks
    printf("param_start: %s", param_start);
    char *first_param = strtok(param_start, "&");
    int i;
    for (i = 0; first_param != NULL; i++)
    {
        char *split = strchr(first_param, '=');
        if (split)
        {
            if (pairsLength == maxPairs)
            {
                maxPairs *= 2;
                HTTPInputParameters *temp = (HTTPInputParameters *)realloc(_ServerHandler->parameters, maxPairs * sizeof(HTTPInputParameters));
                // TODO: Handle fail
                if(temp == NULL)
                return -3;

                _ServerHandler->parameters = temp;
            }
            _ServerHandler->parameters[i].key = strndup(first_param, split - first_param);
            _ServerHandler->parameters[i].value = strndup(split + sizeof(char), strlen(split + sizeof(char)));
            pairsLength++;
            _ServerHandler->parameters->pairsLength = pairsLength;
            printf("key: %s value: %s \n", _ServerHandler->parameters[i].key, _ServerHandler->parameters[i].value);
        }
        first_param = strtok(NULL, "&");
    }
    if(url_cpy != NULL)
    free(url_cpy);

    return 0;
}

RouteFunction HTTPServerHandler_Parse(HTTPServerHandler *_ServerHandler, char *url)
{
    
    int result = HTTPServerHandler_ParseEndPoint(_ServerHandler, url);

    if(result != 0)
    {
        printf("Parsing HTTPServerHandler with url: %d\n", result);
        HTTPServerHandler_Dispose(&_ServerHandler);
        printf("crash %d\n", result);
        return NULL;
    }
    //invalid here
    printf("2\n");
    RouteFunction function = _ServerHandler->onParse(_ServerHandler->end_point);

    result = HTTPServerHandler_ParseInputParameters(_ServerHandler, url);
    printf("1\n");
    if(result == 0)
    return function;


    printf("freeingfds\n");
    HTTPServerHandler_Dispose(&_ServerHandler);
    return NULL;
}

RouteFunction HTTPServerHandler_run(HTTPServerHandler *_ServerHandler, char *url)
{
    if(_ServerHandler == NULL || url == NULL)
    return NULL;

    return HTTPServerHandler_Parse(_ServerHandler, url);
}

void HTTPServerHandler_Dispose(HTTPServerHandler **_ServerHandler)
{
    if (_ServerHandler == NULL || *_ServerHandler == NULL)
        return;

    HTTPServerHandler *handler = *_ServerHandler;

    //if(handler->end_point == NULL)
   // printf("crash %s \n", handler->end_point);

    if (handler->end_point != NULL)
        free(handler->end_point);

    for (int i = 0; i < handler->parameters->pairsLength; i++)
    {
        if (handler->parameters[i].key != NULL)
            free(handler->parameters[i].key);

        if (handler->parameters[i].value != NULL)
            free(handler->parameters[i].value);
    }
    
    free(handler->parameters);
    free(handler);
}