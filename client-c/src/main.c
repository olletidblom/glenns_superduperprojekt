#include "libs/city_cache.h"
#include "libs/ui.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jansson/jansson_private.h"
#include "utils/HTTPClient.h"

#define URL_BUFFER_SIZE 200
#define VALUE_BUFFER_SIZE 128
#define MAX_CITIES 10

json_t *ui_fetch_data(const char *url) {
  int result = 0;
  HTTPClient *client = NULL;

  result = HTTPClient_Init(&client);
  if (result != 0) {
    printf("Failed to initialize HTTPClient for City! Errorcode: %i\n", result);
    return NULL;
  }

  json_t *json = HTTPClient_GET(client, url);
  if (json == NULL) {
    printf("HTTP GET request failed!\n");
    HTTPClient_Dispose(&client);
    return NULL;
  }

  json_incref(json);
  HTTPClient_Dispose(&client);

  return json;
}

KeyValuePair *create_key_value_pair(const char *key, const char *value) {
  KeyValuePair *pair = malloc(sizeof(KeyValuePair));
  if (pair == NULL) {
    return NULL;
  }

  pair->key = strdup(key);
  pair->value = strdup(value);

  if (pair->key == NULL || pair->value == NULL) {
    free(pair->key);
    free(pair->value);
    free(pair);
    return NULL;
  }

  return pair;
}

void add_to_list(LinkedList *list, const char *key, const char *value) {
  KeyValuePair *pair = create_key_value_pair(key, value);
  if (pair != NULL) {
    LinkedList_AddLast(list, pair);
  }
}

LinkedList *GetWeatherData(json_t *json) {
  if (json == NULL || !json_is_object(json)) {
    return NULL;
  }

  LinkedList *list = malloc(sizeof(LinkedList));
  if (!list)
    return NULL;
  LinkedList_Initialize(list);

  json_t *values_obj = json_object_get(json, "current");
  json_t *units_obj = json_object_get(json, "current_units");

  if (values_obj && json_is_object(values_obj)) {
    const char *key;
    json_t *val;

    json_object_foreach(values_obj, key, val) {
      char value_buf[VALUE_BUFFER_SIZE];
      const char *unit_str = NULL;

      if (units_obj && json_is_object(units_obj)) {
        json_t *unit = json_object_get(units_obj, key);
        if (unit && json_is_string(unit)) {
          unit_str = json_string_value(unit);
        }
      }

      if (json_is_string(val)) {
        if (unit_str)
          snprintf(value_buf, sizeof(value_buf), "%s %s",
                   json_string_value(val), unit_str);
        else
          snprintf(value_buf, sizeof(value_buf), "%s", json_string_value(val));
      } else if (json_is_integer(val)) {
        if (unit_str)
          snprintf(value_buf, sizeof(value_buf), "%lld %s",
                   json_integer_value(val), unit_str);
        else
          snprintf(value_buf, sizeof(value_buf), "%lld",
                   json_integer_value(val));
      } else if (json_is_real(val)) {
        if (unit_str)
          snprintf(value_buf, sizeof(value_buf), "%.4f %s",
                   json_real_value(val), unit_str);
        else
          snprintf(value_buf, sizeof(value_buf), "%.4f", json_real_value(val));
      } else {
        continue;
      }

      add_to_list(list, key, value_buf);
    }
  }

  const char *root_key;
  json_t *root_val;

  json_object_foreach(json, root_key, root_val) {
    if (strcmp(root_key, "values") == 0 || strcmp(root_key, "units") == 0)
      continue;

    char value_buf[VALUE_BUFFER_SIZE];

    if (json_is_string(root_val)) {
      snprintf(value_buf, sizeof(value_buf), "%s", json_string_value(root_val));
    } else if (json_is_integer(root_val)) {
      snprintf(value_buf, sizeof(value_buf), "%lld",
               json_integer_value(root_val));
    } else if (json_is_real(root_val)) {
      snprintf(value_buf, sizeof(value_buf), "%.4f", json_real_value(root_val));
    } else if (json_is_boolean(root_val)) {
      snprintf(value_buf, sizeof(value_buf), "%s",
               json_is_true(root_val) ? "true" : "false");
    } else if (json_is_null(root_val)) {
      snprintf(value_buf, sizeof(value_buf), "null");
    } else {
      continue;
    }

    add_to_list(list, root_key, value_buf);
  }

  return list;
}

void ui_get_city_data(double latitude, double longitude) {
  char url[URL_BUFFER_SIZE];
  snprintf(url, sizeof(url), "http://goteborg.onvo.se/api/v1/gwd?lat=%f&lon=%f",
           latitude, longitude);
  json_t *json = ui_fetch_data(url);
  LinkedList *city_data = GetWeatherData(json);
  json_decref(json);
  ui_add_city_data(city_data);
}

void ui_get_search_city(const char *input) {
  char url[URL_BUFFER_SIZE];

  char *encoded_input = url_encode(input);
  if (encoded_input == NULL) {
    fprintf(stderr, "Failed to allocate memory for URL encoding\n");
    return;
  }

  snprintf(url, sizeof(url), "http://goteborg.onvo.se/api/v1/geo?city=%s",
           encoded_input);
  free(encoded_input);

  City *cities = malloc(MAX_CITIES * sizeof(City));
  if (cities == NULL) {
    fprintf(stderr, "Failed to allocate memory for cities\n");
    return;
  }

  json_t *root = ui_fetch_data(url);
  if (root == NULL) {
    free(cities);
    return;
  }

  if (!json_is_array(root)) {
    json_decref(root);
    free(cities);
    return;
  }

  int counter = 0;
  size_t i;
  json_t *item;

  json_array_foreach(root, i, item) {
    if (counter >= MAX_CITIES || !json_is_object(item))
      break;

    json_t *jname = json_object_get(item, "name");
    json_t *jlat = json_object_get(item, "latitude");
    json_t *jlon = json_object_get(item, "longitude");

    if (!json_is_string(jname) || !json_is_number(jlat) ||
        !json_is_number(jlon))
      continue;

    cities[counter].name = strdup(json_string_value(jname));
    if (!cities[counter].name) {
      for (int j = 0; j < counter; j++) {
        free(cities[j].name);
      }
      free(cities);
      json_decref(root);
      return;
    }

    cities[counter].latitude = json_number_value(jlat);
    cities[counter].longitude = json_number_value(jlon);

    cache_add_city(&cities[counter]);
    counter++;
  }

  int cached_count = 0;
  City *cached_cities = cache_get_all(&cached_count);

  ui_add_search_city_data(cached_cities, cached_count);

  for (int j = 0; j < counter; j++) {
    free(cities[j].name);
  }
  free(cities);
  json_decref(root);
}

int main() {
  if (cache_init() != 0) {
    fprintf(stderr, "Warning: Failed to initialize city cache\n");
  }

  int cached_count = 0;
  City *cached_cities = cache_get_all(&cached_count);

  start_ui(ui_get_city_data, ui_get_search_city, cached_cities, cached_count);

  cache_save();
  cache_dispose();

  return 0;
}
