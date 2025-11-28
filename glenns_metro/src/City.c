#include "City.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "utils/HTTPClient.h"
#include "utils/utils.h"

//--------------Internal functions----------------

json_t *City_GetWeatherData(City *_City);

//------------------------------------------------

int City_Init(const char *_Name, const char *countryCode, const char *_Latitude, const char *_Longitude,
              City **_CityPtr)
{
  if (_Name == NULL || _CityPtr == NULL)
    return -1;

  City *_City = (City *)malloc(sizeof(City));
  if (_City == NULL)
  {
    printf("Failed to allocate memory for new City\n");
    return -1;
  }

  memset(_City, 0, sizeof(City));

  _City->name = strdup(_Name);
  if (_City->name == NULL)
  {
    printf("Failed to allocate memory for City name\n");
    free(_City);
    return -1;
  }

  if (countryCode != NULL)
  {
    _City->countryCode = strdup(countryCode);
  }
  else
  {
    _City->countryCode = strdup("");
  }

  if (_City->countryCode == NULL)
  {
    printf("Failed to allocate memory for City name\n");
    free(countryCode);
    return -1;
  }

  if (_Latitude != NULL)
    _City->latitude = strtof(_Latitude, NULL);
  else
    _City->latitude = 0.0f;

  if (_Longitude != NULL)
    _City->longitude = strtof(_Longitude, NULL);
  else
    _City->longitude = 0.0f;

  *(_CityPtr) = _City;

  return 0;
}

int City_GetValue(City *_City, const char *_Name, float *_Value,
                  char _Unit[16])
{
  if (_City == NULL || _Name == NULL)
    return -1;

  json_t *weather = City_GetWeatherData(_City);
  if (weather == NULL)
  {
    printf("Failed to get weather data for City %s\n", _City->name);
    return -2;
  }

  json_t *current = json_object_get(weather, "current");
  if (current == NULL)
  {
    printf("No 'current' field in weather data for City %s\n", _City->name);
    json_decref(weather);
    return -3;
  }

  json_t *current_units = json_object_get(weather, "current_units");
  if (current_units == NULL)
  {
    printf("No 'current_units' field in weather data for City %s\n",
           _City->name);
    json_decref(weather);
    return -4;
  }

  json_t *json_value = json_object_get(current, _Name);
  if (json_value == NULL)
  {
    printf("No '%s' field in weather data for City %s\n", _Name, _City->name);
    json_decref(weather);
    return -5;
  }

  *(_Value) = (float)json_number_value(json_value);

  json_decref(weather);
  return 0;
}

// Skriv ut json till cities/<cityname>.json
int City_WriteWeatherData(City *_City, json_t *weatherJson)
{
  if (_City == NULL || weatherJson == NULL)
  {
    printf("City_WriteWeatherData: invalid input\n");
    return -1;
  }

  char filename[256];
  snprintf(filename, sizeof(filename), "cities/%s.json", _City->name);

  int result = json_dump_file(weatherJson, filename,
                              JSON_INDENT(4) | JSON_PRESERVE_ORDER);
  if (result != 0)
  {
    printf(
        "Failed to write weather data for City %s to file %s! Errorcode: %i\n",
        _City->name, filename, result);
    return result;
  }

  return 0; // success
}

json_t *City_GetWeatherData(City *_City)
{
  // printf("start of function, city name is %s\n", _City->name);
  // if city has weather data is cached
  json_error_t error;

  // Variables to use when comparing time
  char cache_filepath[256];
  snprintf(cache_filepath, sizeof(cache_filepath), "cities/%s.json",
           _City->name);

  struct stat filinfo;
  if (stat(cache_filepath, &filinfo) == 0) // file exists
  {
    time_t now = time(NULL);
    double age = difftime(
        now, filinfo.st_mtime); // st_time är last modified time av filen
    if (age < 900)              // 15 minutes
    {
      // printf("Loading cached response from %s (age %.0f seconds)\n",
      // cache_filepath, age);

      json_t *root = json_load_file(cache_filepath, 0, NULL);
      // Now checking if loaded data has "temperature"
      json_t *temperature = json_object_get(root, "current");
      temperature = json_object_get(temperature, "temperature_2m");
      if (temperature == NULL)
      {
        // If temperature does not exist, we need to fetch new data
        json_decref(root);
        printf("No cached temperature data for City %s, fetching new data...\n",
               _City->name);
      }
      else if (root != NULL)
      {
        return root; // fresh cache
      }
      else
      {
        printf("Cache file invalid, fetching new data..\n");
        json_decref(root);
      }
    }
    else
    {
      printf("Cache file too old (age %.0f seconds), fetching new data..\n",
             age);
    }
  }
  else
  {
    printf("Cache file does not exist, fetching new data.\n");
  }

  int result = 0;
  HTTPClient *client = NULL;

  result = HTTPClient_Init(&client);
  if (result != 0)
  {
    printf("Failed to initialize HTTPClient for City! Errorcode: %i\n", result);
    City_Dispose(&_City);
    return NULL;
  }

  json_t *cords = HTTPClient_GetCords(client, _City->name, _City->countryCode);
  if (cords == NULL)
  {
    printf("HTTP GET request failed! Errorcode: %i\n", errno);
    HTTPClient_Dispose(&client);
    return NULL;
  }


  json_t *results = json_object_get(cords, "results");
  if (!json_is_array(results) || json_array_size(results) == 0)
  {
    printf("No results found for city %s\n", _City->name);
    HTTPClient_Dispose(&client);
    return NULL;
  }

  json_t *first_result = json_array_get(results, 0);

  json_t *latitude_json = json_object_get(first_result, "latitude");

  json_t *longitude_json = json_object_get(first_result, "longitude");

  _City->latitude = (float)json_number_value(latitude_json);

  _City->longitude = (float)json_number_value(longitude_json);

  json_decref(cords);
  char url[512];
  snprintf(url, sizeof(url), CITY_WEATHER_API_URL, _City->latitude,
           _City->longitude);


  HTTPClient_Dispose(&client);

  result = HTTPClient_Init(&client);
  if (result != 0)
  {
    printf("Failed to initialize HTTPClient for City! Errorcode: %i\n", result);
    City_Dispose(&_City);
    return NULL;
  }

  json_t *json = HTTPClient_GetWeather(client, url);
  if (json == NULL)
  {
    printf("HTTP GET request failed! Errorcode: %i\n", errno);
    HTTPClient_Dispose(&client);
    return NULL;
  }

  json_t *ret = json_incref(json);
  // Lägg till de tre attributen vi snackade om

  json_t *city_variables = json_object();
  json_object_set_new(city_variables, "name", json_string(_City->name));
  json_object_set_new(city_variables, "latitude", json_real((double)_City->latitude));
  json_object_set_new(city_variables, "longitude", json_real((double)_City->longitude));

  // skriv ut till fil
  City_WriteWeatherData(_City, city_variables);

  json_decref(city_variables);
  printf("Fetched weather data for City %s\n", _City->name);
  HTTPClient_Dispose(&client);

  return json;
}

void City_Dispose(City **_CityPtr)
{
  if (_CityPtr == NULL || *(_CityPtr) == NULL)
    return;

  City *_City = *(_CityPtr);

  if (_City->name != NULL)
    free(_City->name);

  if (_City->countryCode != NULL)
    free(_City->countryCode);

  free(_City);
  *(_CityPtr) = NULL;
}
