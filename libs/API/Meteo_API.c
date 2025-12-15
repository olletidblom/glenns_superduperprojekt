#include "Meteo_API.h"
#include <stdlib.h>
#include "../../glenns_metro/libs/utils/md5.h"
#include "../../glenns_metro/libs/utils/utils.h"
#include <sys/stat.h>
#include <time.h>

int Meteo_ParseResponse(Meteo_API *Meteo_api, URLHandler *handler);

int Meteo_Init(Meteo_API **_MeteoApiPtr, const char *latitude, const char *longitude)
{
    if (_MeteoApiPtr == NULL || latitude == NULL || longitude == NULL)
        return -1;

    Meteo_API *Meteo_api = (Meteo_API *)malloc(sizeof(Meteo_API));
    if (Meteo_api == NULL)
        return -2;

    Meteo_api->latitude = strdup(latitude);
    if (Meteo_api->latitude == NULL)
    {
        free(Meteo_api);
        return -3;
    }

    Meteo_api->longitude = strdup(longitude);
    if (Meteo_api->longitude == NULL)
    {
        free(Meteo_api->latitude);
        free(Meteo_api);
        return -3;
    }

    Meteo_api->curl_response = NULL;
    Meteo_api->url = NULL;
    Meteo_api->file_path = NULL;
    Meteo_api->parsed_response = NULL;
    Meteo_api->response_data = NULL;
    create_folder("cache");
    // HTTPClient_Initiate(&Meteo_api->http_client);

    *_MeteoApiPtr = Meteo_api;
    return 0;
}

int Meteo_BuildRequestURL(Meteo_API *Meteo_api)
{
    if (Meteo_api == NULL)
        return -1;

    char buffer[1024];
    int length = snprintf(buffer, sizeof(buffer), "https://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m", Meteo_api->latitude, Meteo_api->longitude);

    if (length <= 0)
        return -2;

    Meteo_api->url = strdup(buffer);

    return 0;
}

int Meteo_CheckCache(Meteo_API *Meteo_api, URLHandler *handler)
{
    const char *hash_name = MD5_HashToString(Meteo_api->latitude, strlen(Meteo_api->latitude));
    printf("url: %s\nmd5: %s\n", Meteo_api->latitude, hash_name);

    char cache_filepath[256];
    snprintf(cache_filepath, sizeof(cache_filepath), "cache/%s_Meteo.json", hash_name);

    Meteo_api->file_path = strdup(cache_filepath);

    printf("Cache filepath: %s\n", cache_filepath);
    struct stat filinfo;
    if (stat(cache_filepath, &filinfo) == 0)
    {

        time_t nu = time(NULL);
        double skillnad = difftime(nu, filinfo.st_mtime);
        if (skillnad < 900) // 15 minutes cache
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
                Meteo_api->response_data = json_dumps(json, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
                if (Meteo_api->response_data != NULL)
                {
                    Meteo_ParseResponse(Meteo_api, handler);
                    json_decref(json);
                    return 0;
                }
            }
        }
        return -1;
    }
    return -2;
}

int Meteo_CacheResponse(Meteo_API *Meteo_api)
{
    if (Meteo_api == NULL || Meteo_api->response_data == NULL)
        return -1;

    json_t *json = json_loads(Meteo_api->response_data, 0, NULL);
    if (json == NULL)
    {
        return -2;
    }

    int result = json_dump_file(json, Meteo_api->file_path, JSON_INDENT(4));
    if (result != 0)
    {
        json_decref(json);
        return -3;
    }

    json_decref(json);

    return 0;
}

int Meteo_ParseResponse(Meteo_API *Meteo_api, URLHandler *handler)
{

    if (Meteo_api == NULL || Meteo_api->response_data == NULL)
        return -1;

    json_t *json = json_loads(Meteo_api->response_data, 0, NULL);
    if (json == NULL)
    {
        return -2;
    }

    json_t *Meteo_data = json_object_get(json, "current");
    const char *key;
    json_t *value;

    json_t* response = json_object();


    // If no specifed parameters, return all data
    json_object_foreach(Meteo_data, key, value)
    {
        if (handler->parameters->pairsLength == 2)
        {

            if (json_is_integer(value))
            {

                json_object_set_new(response, key, json_integer(json_integer_value(value)));
            }
            else if (json_is_string(value))
            {

                json_object_set_new(response, key, json_string(json_string_value(value)));
            }
            else if (json_is_real(value))
            {

                json_object_set_new(response, key, json_real(json_real_value(value)));
            }

        }
    }

    // Compare requested parameters with available data
    for (int i = 0; i < handler->parameters->pairsLength; i++)
    {
        json_object_foreach(Meteo_data, key, value)
        {

            if (strcmp(handler->parameters[i].key, key) == 0)
            {

                if (json_is_integer(value))
                {

                    json_object_set_new(response, key, json_integer(json_integer_value(value)));
                }
                else if (json_is_string(value))
                {

                    json_object_set_new(response, key, json_string(json_string_value(value)));
                }
                else if (json_is_real(value))
                {

                    json_object_set_new(response, key, json_real(json_real_value(value)));
                }

            }
        }
    }

    Meteo_api->parsed_response = json_dumps(response, JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    json_decref(response);
    json_decref(json);
    json_decref(Meteo_data);
    return 0;
}

int Meteo_SendRequest(Meteo_API *Meteo_api, URLHandler *handler)
{

    int result = Meteo_BuildRequestURL(Meteo_api);
    if (result != 0)
        return result;

    result = Meteo_CheckCache(Meteo_api, handler);
    if (result == 0)
    {
        // Cached data found
        return 0;
    }

    Meteo_api->curl_response = Curl_HTTPGet(Meteo_api->url);

    if (Meteo_api->curl_response == NULL || Meteo_api->curl_response->data == NULL)
        return -1;

    Meteo_api->response_data = strdup(Meteo_api->curl_response->data);

    Meteo_CacheResponse(Meteo_api);

    Meteo_ParseResponse(Meteo_api, handler);

    if (Meteo_api->response_data == NULL)
        return -2;

    return 0;
}

void Meteo_Dispose(Meteo_API **_MeteoApiPtr)
{
    if (_MeteoApiPtr == NULL || *_MeteoApiPtr == NULL)
        return;

    Meteo_API *Meteo_api = *_MeteoApiPtr;

    if (Meteo_api->latitude != NULL)
        free(Meteo_api->latitude);

    if (Meteo_api->longitude != NULL)
        free(Meteo_api->longitude);

    if (Meteo_api->response_data != NULL)
        free(Meteo_api->response_data);

    if (Meteo_api->parsed_response != NULL)
        free(Meteo_api->parsed_response);

    if (Meteo_api->url != NULL)
        free(Meteo_api->url);

    if (Meteo_api->file_path != NULL)
        free(Meteo_api->file_path);

    if (Meteo_api->curl_response != NULL)
        Curl_Dispose(&Meteo_api->curl_response);

    free(Meteo_api);
    *_MeteoApiPtr = NULL;
}