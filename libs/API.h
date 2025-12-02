#ifndef API_H
#define API_H

#include "HTTP.h"

// Simple geocoding API function - returns raw JSON data
char* API_get_geocoding_json(const char* city_name, double latitude, double longitude);

// Helper functions
void API_free_json(char* json_data);
char* API_url_encode(const char* input);

#endif
