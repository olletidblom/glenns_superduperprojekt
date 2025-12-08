#include "GEO_API.h"
#include <stdlib.h>
#include "../../glenns_metro/libs/utils/md5.h"
#include "../../glenns_metro/libs/utils/utils.h"

#include <sys/stat.h>


int GEO_ParseResponse(GEO_API *geo_api);

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


    geo_api->response = NULL;
    geo_api->url = NULL;
    geo_api->file_path = NULL;
    geo_api->result = NULL;

    create_folder("cache");
    //HTTPClient_Initiate(&geo_api->http_client);


    *_GeoApiPtr = geo_api;
    return 0;
}

int GEO_BuildRequestURL(GEO_API *geo_api)
{
    if (geo_api == NULL || geo_api->city_name == NULL)
        return -1;

    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer), "geocoding-api.open-meteo.com/v1/search?name=%s", geo_api->city_name);

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
    snprintf(cache_filepath, sizeof(cache_filepath), "cache/%s_GEO.json", hash_name);

    geo_api->file_path = strdup(cache_filepath);

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
            {
                GEO_ParseResponse(geo_api);
                json_decref(json);
                return 0;
            }
        }
    }
    return -2;
}

int GEO_CacheResponse(GEO_API *geo_api)
{
    if (geo_api == NULL || geo_api->city_name == NULL || geo_api->result == NULL)
        return -1;

    json_t *json = json_loads(geo_api->result, 0, NULL);
    if (json == NULL)
    {
        return -2;
    }

    int result = json_dump_file(json, geo_api->file_path, JSON_INDENT(4));
    if (result != 0)
    {
        json_decref(json);
        return -3;
    }

    json_t *geo_data = json_object_get(json, "results");
    json_t* results_array = json_array_get(geo_data, 0);
    json_t *lat = json_object_get(results_array, "latitude");
    json_t *lon = json_object_get(results_array, "longitude");

    json_t *city_variables = json_object();

    float latitude = (float)json_number_value(lat);
    float longitude = (float)json_number_value(lon);

    json_object_set_new(city_variables, "name", json_string(geo_api->city_name));
    json_object_set_new(city_variables, "latitude", json_real(latitude));
    json_object_set_new(city_variables, "longitude", json_real(longitude));

    char filename[256];
    snprintf(filename, sizeof(filename), "cities/%s.json", geo_api->city_name);

    json_dump_file(city_variables, filename, JSON_INDENT(4));
    json_decref(city_variables);
    json_decref(json);

    return 0;
}

int GEO_ParseResponse(GEO_API *geo_api)
{

    char buffer[2048] = {0};
    if (geo_api == NULL || geo_api->result == NULL)
        return -1;

    json_t *json = json_loads(geo_api->result, 0, NULL);
    if (json == NULL)
    {
        return -2;
    }

    json_t *geo_data = json_object_get(json, "results");
    size_t array_size = json_array_size(geo_data);

    for(size_t i = 0; i < array_size; i++)
    {
        json_t *result = json_array_get(geo_data, i);
        json_t *name = json_object_get(result, "name");
        json_t *latitude = json_object_get(result, "latitude");
        json_t *longitude = json_object_get(result, "longitude");

        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Result %zu: %s (Lat: %.6f, Lon: %.6f)\n", i + 1, json_string_value(name), json_number_value(latitude), json_number_value(longitude));
    }

    free(geo_api->result);

    geo_api->result = strdup(buffer);

    json_decref(json);
    return 0;
}

int GEO_SendRequest(GEO_API *geo_api)
{

    if (GEO_BuildRequestURL(geo_api) == 0 && GEO_CheckCache(geo_api) != 0)
    {
        geo_api->response = Curl_HTTPGet(geo_api->url);
        
        if(geo_api->response == NULL || geo_api->response->data == NULL)
            return -1;

        geo_api->result = strdup(geo_api->response->data);
        GEO_CacheResponse(geo_api);
        GEO_ParseResponse(geo_api);

        if (geo_api->result == NULL)
            return -2;
    }
    return 0;
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
    
    if (geo_api->url != NULL)
        free(geo_api->url);
    
    if (geo_api->file_path != NULL)
        free(geo_api->file_path);

    if(geo_api->response != NULL)
        Curl_Dispose(&geo_api->response);
    // TODO:
    // Dispose http_client
    free(geo_api);
    *_GeoApiPtr = NULL;
}