#include "libs/city_cache.h"
#include "libs/ui.h"
#include "libs/weather_api.h"
#include <stdio.h>

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
