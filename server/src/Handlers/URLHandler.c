#include "URLHandler.h"
#include "RouteHandler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
AnonymFunkion* URLHandler_run = ChooseAPI_return_funktion; ->
weather__URLHandler -> URLHandler_run();
*/

void URLHandler_Dispose(URLHandler **_URLHandler);

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