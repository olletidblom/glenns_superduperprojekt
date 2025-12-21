#include "libs/ui.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jansson/jansson_private.h"
#include "utils/HTTPClient.h"

#define GWDURL "http://kontoret.onvo.se:10180/api/v1/gwd?city=G%C3%B6teborg&CountryCode=SE"

json_t *ui_fetch_data(const char* url) {
    int result = 0;
    double latitude = 0;
    double longitude = 0;
    char *name = "Göteborg";
    HTTPClient *client = NULL;

    result = HTTPClient_Init(&client);
    if (result != 0) {
        printf("Failed to initialize HTTPClient for City! Errorcode: %i\n", result);
        return NULL;
    }

    // char url[512];
    // snprintf(url, sizeof(url), CITY_WEATHER_API_URL, latitude,
    //          longitude);

    json_t *json = HTTPClient_GET(client, url);
    if (json == NULL) {
        printf("HTTP GET request failed! Errorcode: %i\n", errno);
        HTTPClient_Dispose(&client);
        return NULL;
    }

    json_t *ret = json_incref(
        json); // This means that the caller is responsible for disposing the json
    // object. It will not be disposed when the HTTPClient is disposed.

    // Lägg till de tre attributen vi snackade om
    json_object_set_new(ret, "name", json_string(name));
    json_object_set_new(ret, "latitude", json_real((double) latitude));
    json_object_set_new(ret, "longitude", json_real((double) longitude));

    HTTPClient_Dispose(&client);

    return ret;
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

    // Allocate the list
    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) return NULL;
    LinkedList_Initialize(list);

    // Get "values" and "units" objects
    json_t *values_obj = json_object_get(json, "current");
    json_t *units_obj = json_object_get(json, "current_units");

    if (values_obj && json_is_object(values_obj)) {
        const char *key;
        json_t *val;

        // Iterate over all keys in "values"
        json_object_foreach(values_obj, key, val) {
            char value_buf[128];
            const char *unit_str = NULL;

            // Get corresponding unit if exists
            if (units_obj && json_is_object(units_obj)) {
                json_t *unit = json_object_get(units_obj, key);
                if (unit && json_is_string(unit)) {
                    unit_str = json_string_value(unit);
                }
            }

            // Convert value to string
            if (json_is_string(val)) {
                if (unit_str)
                    snprintf(value_buf, sizeof(value_buf), "%s %s", json_string_value(val), unit_str);
                else
                    snprintf(value_buf, sizeof(value_buf), "%s", json_string_value(val));
            } else if (json_is_integer(val)) {
                if (unit_str)
                    snprintf(value_buf, sizeof(value_buf), "%lld %s", json_integer_value(val), unit_str);
                else
                    snprintf(value_buf, sizeof(value_buf), "%lld", json_integer_value(val));
            } else if (json_is_real(val)) {
                if (unit_str)
                    snprintf(value_buf, sizeof(value_buf), "%.4f %s", json_real_value(val), unit_str);
                else
                    snprintf(value_buf, sizeof(value_buf), "%.4f", json_real_value(val));
            } else {
                continue; // skip unsupported types
            }

            add_to_list(list, key, value_buf);
        }
    }

    // Now add all other top-level keys that are NOT "values" or "units"
    const char *root_key;
    json_t *root_val;

    json_object_foreach(json, root_key, root_val) {
        if (strcmp(root_key, "values") == 0 || strcmp(root_key, "units") == 0)
            continue; // skip, already processed

        char value_buf[128];

        if (json_is_string(root_val)) {
            snprintf(value_buf, sizeof(value_buf), "%s", json_string_value(root_val));
        } else if (json_is_integer(root_val)) {
            snprintf(value_buf, sizeof(value_buf), "%lld", json_integer_value(root_val));
        } else if (json_is_real(root_val)) {
            snprintf(value_buf, sizeof(value_buf), "%.4f", json_real_value(root_val));
        } else if (json_is_boolean(root_val)) {
            snprintf(value_buf, sizeof(value_buf), "%s", json_is_true(root_val) ? "true" : "false");
        } else if (json_is_null(root_val)) {
            snprintf(value_buf, sizeof(value_buf), "null");
        } else {
            continue; // skip arrays/objects for now
        }

        add_to_list(list, root_key, value_buf);
    }

    return list;
}

void ui_get_city_data(const char *input) {
    json_t *json = ui_fetch_data(GWDURL);
    LinkedList *city_data = GetWeatherData(json);
    ui_add_city_data(city_data);
}

void ui_get_city_data_new(double latitude, double longitude) {
    char url[150];
    snprintf(url, 149, "http://kontoret.onvo.se:10180/api/v1/gwd?lat=%f&lon=%f", latitude, longitude);
    json_t *json = ui_fetch_data(url);
    LinkedList *city_data = GetWeatherData(json);
    ui_add_city_data(city_data);
}

void ui_get_search_city(const char *input) {
    char url[150];
    snprintf(url, 149, "http://kontoret.onvo.se:10180/api/v1/geo?city=\"input\"", input);
    City *cities = malloc(3 * sizeof(City));

    cities[0] = (City){ "Stockholm", 1.2, 3.4 };
    cities[1] = (City){ "Kiruna", 5.6, 7.8 };
    cities[2] = (City){ "Lysekil", 9.0, 1.2 };

    int number_of_cities = sizeof(cities) / sizeof(City);

    ui_add_search_city_data(cities, number_of_cities);
}

int main() {
    //json_t* json = ui_fetch_data();
    //LinkedList *list = Cities_GetCityValues(json);
    //KeyValuePair *kvp = NULL;
    //LinkedList_ForEach(list, &kvp) {
    //  printf("%s : %s\n\n", kvp->key, kvp->value);
    //}
    start_ui(ui_get_city_data, ui_get_search_city);

    return 0;
}
