#ifndef WEATHER_API_H
#define WEATHER_API_H

#include "jansson/jansson.h"
#include "utils/LinkedList.h"
#include "city_cache.h"

json_t *fetch_json(const char *url);

LinkedList *get_weather_data(json_t *json);

int get_city_data(json_t *root, City *cities, int max_cities);

void ui_get_city_data(double latitude, double longitude);

void ui_get_search_city(const char *input);

#endif
