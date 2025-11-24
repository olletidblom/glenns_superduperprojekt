#include "HTTPServerHandler.h"
#include <stdlib.h>
#include <string.h>

/*
AnonymFunkion* HTTPServerHandler_run = ChooseAPI_return_funktion; ->
weather_handler -> HTTPServerHandler_run();
*/

void HTTPServerHandler_Parse() {
    //ParseEndpoint
    //ParseInput
}

void HTTPServerHandler_run() {

}

void HTTPServerHandler_ParseInputParameters(const char *url)
{
    int pairsLength = 0;
    int maxPairs = 2;
    HTTPServerHandler* handler = (HTTPServerHandler*)malloc(sizeof(HTTPServerHandler));
    handler->parameters = (HTTPInputParameters*)malloc(maxPairs * sizeof(HTTPInputParameters));
    char *url_cpy = NULL;

    if (url != NULL)
        url_cpy = strndup(url, strlen(url));
   
    char *param_start = strchr(url_cpy, '?'); 
    if (param_start != NULL)
    {
        
        printf("Param_start = %s : url_cpy = %p : url_cpy_length = %d\n", param_start, url_cpy, strlen(url_cpy));
        param_start++;
    }

    //TODO: Add error checks
    printf("param_start: %s", param_start);
    char* first_param = strtok(param_start, "&");
    int i;
    for(i = 0; first_param != NULL; i++)
    {
        char* split = strchr(first_param, '=');
        if(split) {
            if(pairsLength == maxPairs) {
                maxPairs *= 2;
                HTTPInputParameters* temp = (HTTPInputParameters*)realloc(handler->parameters,maxPairs * sizeof(HTTPInputParameters));
                //TODO: Handle fail
                if(temp) handler->parameters = temp;
            }
             handler->parameters[i].key = strndup(first_param, split - first_param);
             handler->parameters[i].value = strndup(split+sizeof(char), strlen(split+sizeof(char)));
             pairsLength++;
             printf("key: %s value: %s \n", handler->parameters[i].key, handler->parameters[i].value);
             
        }
        first_param = strtok(NULL, "&");
    }

    
}