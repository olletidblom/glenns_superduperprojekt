#include "GEO_API.h"
#include <stdlib.h>
#include "../glenns_metro/libs/utils/md5.h"
#include <sys/stat.h>

int GEO_Init(GEO_API **_GeoApiPtr, const char *city_name)
{
    if (_GeoApiPtr == NULL || city_name == NULL)
        return -1;

    GEO_API *geo_api = (GEO_API *)malloc(sizeof(GEO_API));
    if (geo_api == NULL)
        return -2;

    geo_api->city_name = strdup(city_name);
    if (geo_api->city_name == NULL)
    {
        free(geo_api);
        return -3;
    }
    
    HTTPClient_Initiate(&geo_api->http_client);
    geo_api->result = NULL;
    
    *_GeoApiPtr = geo_api;
    return 0;
}

int GEO_BuildRequestURL(GEO_API *geo_api)
{
    if (geo_api == NULL || geo_api->city_name == NULL)
        return -1;

    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer), "/v1/search?name=%s", geo_api->city_name);

    if (length <= 0)
        return -2;

    geo_api->url = strdup(buffer);

    return 0;
}

int GEO_CheckCache(GEO_API *geo_api)
{
    const char *hash_name = MD5_HashToString(geo_api->city_name, strlen(geo_api->city_name));
    printf("url: %s\nmd5: %s\n", geo_api->city_name, hash_name);

    char cache_filepath[256];
    snprintf(cache_filepath, sizeof(cache_filepath), "cache/%s.json", hash_name);

    printf("Cache filepath: %s\n", cache_filepath);
    struct stat filinfo;
    if (stat(cache_filepath, &filinfo) == 0)
    {
        printf("Loading cached response from %s\n", cache_filepath);
        json_t *json = json_load_file(cache_filepath, 0, NULL);
        if (json == NULL)
        {
            printf("Failed to parse cached JSON response! Removing cache file.\n");
            remove(cache_filepath);
            return -1;
        }
        else
        {
            geo_api->result = json_dumps(json, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
            if (geo_api->result != NULL)
                return 0;
        }
    }
    return -2;
}

int GEO_SendRequest(GEO_API *geo_api)
{

    if (GEO_BuildRequestURL(geo_api) == 0)
    {
        HTTPClient_GET(&geo_api->http_client, geo_api->url, "geocoding-api.open-meteo.com", NULL);
        geo_api->result = geo_api->http_client.buffer;

        if(geo_api->result == NULL)
        return -1;
    }
}

void GEO_Dispose(GEO_API **_GeoApiPtr)
{
    if (_GeoApiPtr == NULL || *_GeoApiPtr == NULL)
        return;

    GEO_API *geo_api = *_GeoApiPtr;

    if (geo_api->city_name != NULL)
        free(geo_api->city_name);

    if (geo_api->result != NULL)
        free(geo_api->result);

    //TODO:
    //Dispose http_client
    free(geo_api);
    *_GeoApiPtr = NULL;
}