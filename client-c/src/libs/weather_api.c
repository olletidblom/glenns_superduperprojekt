#include "weather_api.h"

#include "curl.h"
#include "ui.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_BUFFER_SIZE 200
#define VALUE_BUFFER_SIZE 128
#define MAX_CITIES 10

json_t *fetch_json(const char *url) {
  CurlResponse *response = Curl_HTTPGet(url);
  if (response == NULL) {
    fprintf(stderr, "HTTP GET request failed!\n");
    return NULL;
  }

  json_t *json = json_loads(response->data, 0, NULL);
  Curl_Dispose(&response);

  if (json == NULL) {
    fprintf(stderr, "Failed to parse JSON response\n");
    return NULL;
  }

  return json;
}

static KeyValuePair *create_key_value_pair(const char *key, const char *value) {
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

static int add_to_list(LinkedList *list, const char *key, const char *value) {
  KeyValuePair *pair = create_key_value_pair(key, value);
  if (pair == NULL) {
    return -1;
  }
  LinkedList_AddLast(list, pair);
  return 0;
}

LinkedList *get_weather_data(json_t *json) {
  if (json == NULL || !json_is_object(json)) {
    return NULL;
  }

  LinkedList *list = malloc(sizeof(LinkedList));
  if (!list)
    return NULL;
  LinkedList_Initialize(list);

  const char *key;
  json_t *val;

  json_object_foreach(json, key, val) {
    char value_buf[VALUE_BUFFER_SIZE];

    if (json_is_string(val)) {
      snprintf(value_buf, sizeof(value_buf), "%s", json_string_value(val));
    } else if (json_is_integer(val)) {
      snprintf(value_buf, sizeof(value_buf), "%lld", json_integer_value(val));
    } else if (json_is_real(val)) {
      snprintf(value_buf, sizeof(value_buf), "%.2f", json_real_value(val));
    } else {
      continue;
    }

    if (add_to_list(list, key, value_buf) != 0) {
      KeyValuePair *pair = NULL;
      while (list->length > 0) {
        pair = (KeyValuePair *)LinkedList_RemoveFirst(list);
        if (pair != NULL) {
          free(pair->key);
          free(pair->value);
          free(pair);
        }
      }
      free(list);
      return NULL;
    }
  }

  return list;
}

void ui_get_city_data(double latitude, double longitude) {
  char url[URL_BUFFER_SIZE];
  snprintf(url, sizeof(url), "http://goteborg.onvo.se/api/v1/gwd?lat=%f&lon=%f",
           latitude, longitude);
  json_t *json = fetch_json(url);
  LinkedList *city_data = get_weather_data(json);
  json_decref(json);
  ui_add_city_data(city_data);
}

int get_city_data(json_t *root, City *cities, int max_cities) {
  if (root == NULL || !json_is_array(root) || cities == NULL) {
    return -1;
  }

  int counter = 0;
  size_t i;
  json_t *item;

  json_array_foreach(root, i, item) {
    if (counter >= max_cities || !json_is_object(item))
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
      return -1;
    }

    cities[counter].latitude = json_number_value(jlat);
    cities[counter].longitude = json_number_value(jlon);

    cache_add_city(&cities[counter]);
    counter++;
  }

  return counter;
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

  json_t *root = fetch_json(url);
  if (root == NULL) {
    free(cities);
    return;
  }

  int counter = get_city_data(root, cities, MAX_CITIES);
  if (counter < 0) {
    json_decref(root);
    free(cities);
    return;
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
