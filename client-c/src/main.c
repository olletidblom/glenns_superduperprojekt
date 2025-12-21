#include "libs/ui.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jansson/jansson_private.h"
#include "utils/HTTPClient.h"

json_t *ui_fetch_data(const char* url) {
    int result = 0;
    HTTPClient *client = NULL;

    result = HTTPClient_Init(&client);
    if (result != 0) {
        printf("Failed to initialize HTTPClient for City! Errorcode: %i\n", result);
        return NULL;
    }

    json_t *json = HTTPClient_GET(client, url);
    if (json == NULL) {
        printf("HTTP GET request failed! Errorcode: %i\n", errno);
        HTTPClient_Dispose(&client);
        return NULL;
    }

    json_t *ret = json_incref(json);

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

void ui_get_city_data(double latitude, double longitude) {
    //https://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=temperature_2m
    char url[200];
    snprintf(url, 199, "http://goteborg.onvo.se/api/v1/gwd?lat=%f&lon=%f", latitude, longitude);
    json_t *json = ui_fetch_data(url);
    LinkedList *city_data = GetWeatherData(json);
    ui_add_city_data(city_data);
}

void ui_get_search_city(const char *input) {
    const int MAX_CITIES = 10;
    char url[200];
    snprintf(url, 199, "http://goteborg.onvo.se/api/v1/geo?city=%s", input);

    City *cities = malloc(MAX_CITIES * sizeof(City));

    json_t *root = ui_fetch_data(url);

    json_t *results = json_object_get(root, "results");
    size_t i;
    json_t *item;

    if (!json_is_array(results))
        return;

    int counter = 0;

    json_array_foreach(results, i, item) {
        if (i >= MAX_CITIES || !json_is_object(item))
            break;

        json_t *jname = json_object_get(item, "name");
        json_t *jlat  = json_object_get(item, "latitude");
        json_t *jlon  = json_object_get(item, "longitude");

        if (!json_is_string(jname) ||
            !json_is_number(jlat)  ||
            !json_is_number(jlon))
            continue;

        cities[i].name = strdup(json_string_value(jname));
        if (!cities[i].name)
            return;

        cities[i].latitude  = json_number_value(jlat);
        cities[i].longitude = json_number_value(jlon);
        counter++;
    }

    ui_add_search_city_data(cities, counter);
}

int main() {
    start_ui(ui_get_city_data, ui_get_search_city);

    return 0;
}
