#include "Parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>





int Parser_FindInString(char* source, char* key, char* _OutValue, size_t bufferSize)
{
    char* key_start = strstr(source, key);

    if(key_start != NULL)
    {
        key_start = key_start + strlen(key);
    }
    else
    {
        return -1;
    }

    char* value_end = strchr(key_start, '&');

    if(value_end != NULL)
    {
        int value_length = value_end - key_start;

        key_start[value_length] = '\0';
        snprintf(_OutValue, bufferSize, "%s", key_start);
        key_start[value_length] = '&';
        return 0;
    }
    else
    {
        snprintf(_OutValue, bufferSize, "%s", key_start);
        return 0;
    }
    return -1;
}