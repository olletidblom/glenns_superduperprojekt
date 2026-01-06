#include "city_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_FILE "cities.cache"
#define INITIAL_CAPACITY 100
#define MAX_LINE_LENGTH 512

// Internal cache structure
static City *cache_cities = NULL;
static int cache_count = 0;
static int cache_capacity = 0;

// Compare function for qsort
static int compare_cities(const void *a, const void *b) {
  const City *c1 = (const City *)a;
  const City *c2 = (const City *)b;
  return strcmp(c1->name, c2->name);
}

// Check if two cities are the same (by lat/lon)
static int cities_equal(const City *c1, const City *c2) {
  return c1->latitude == c2->latitude && c1->longitude == c2->longitude;
}

// Expand cache capacity if needed
static int expand_cache(void) {
  int new_capacity =
      cache_capacity == 0 ? INITIAL_CAPACITY : cache_capacity * 2;
  City *new_cities = realloc(cache_cities, new_capacity * sizeof(City));
  if (new_cities == NULL) {
    return -1;
  }
  cache_cities = new_cities;
  cache_capacity = new_capacity;
  return 0;
}

int cache_init(void) {
  // Allocate initial capacity
  cache_cities = malloc(INITIAL_CAPACITY * sizeof(City));
  if (cache_cities == NULL) {
    return -1;
  }
  cache_capacity = INITIAL_CAPACITY;
  cache_count = 0;

  // Try to load from file
  FILE *file = fopen(CACHE_FILE, "r");
  if (file == NULL) {
    // File doesn't exist yet, that's okay
    return 0;
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Remove newline
    line[strcspn(line, "\n")] = '\0';

    // Parse line: name|latitude|longitude
    char *name = strtok(line, "|");
    char *lat_str = strtok(NULL, "|");
    char *lon_str = strtok(NULL, "|");

    if (name == NULL || lat_str == NULL || lon_str == NULL) {
      continue; // Skip malformed lines
    }

    // Expand if needed
    if (cache_count >= cache_capacity) {
      if (expand_cache() != 0) {
        fclose(file);
        return -1;
      }
    }

    // Add to cache
    cache_cities[cache_count].name = strdup(name);
    cache_cities[cache_count].latitude = atof(lat_str);
    cache_cities[cache_count].longitude = atof(lon_str);

    if (cache_cities[cache_count].name == NULL) {
      fclose(file);
      return -1;
    }

    cache_count++;
  }

  fclose(file);

  // Sort the loaded cities
  if (cache_count > 0) {
    qsort(cache_cities, cache_count, sizeof(City), compare_cities);
  }

  return 0;
}

int cache_add_city(const City *city) {
  if (city == NULL || city->name == NULL) {
    return -1;
  }

  // Check if city already exists (deduplicate by lat/lon)
  for (int i = 0; i < cache_count; i++) {
    if (cities_equal(&cache_cities[i], city)) {
      return 0; // Already in cache, nothing to do
    }
  }

  // Expand if needed
  if (cache_count >= cache_capacity) {
    if (expand_cache() != 0) {
      return -1;
    }
  }

  // Add new city
  cache_cities[cache_count].name = strdup(city->name);
  cache_cities[cache_count].latitude = city->latitude;
  cache_cities[cache_count].longitude = city->longitude;

  if (cache_cities[cache_count].name == NULL) {
    return -1;
  }

  cache_count++;

  // Re-sort to maintain alphabetical order
  qsort(cache_cities, cache_count, sizeof(City), compare_cities);

  return 0;
}

City *cache_get_all(int *count) {
  if (count != NULL) {
    *count = cache_count;
  }
  return cache_cities;
}

int cache_save(void) {
  FILE *file = fopen(CACHE_FILE, "w");
  if (file == NULL) {
    return -1;
  }

  for (int i = 0; i < cache_count; i++) {
    fprintf(file, "%s|%f|%f\n", cache_cities[i].name, cache_cities[i].latitude,
            cache_cities[i].longitude);
  }

  fclose(file);
  return 0;
}

void cache_dispose(void) {
  // Free all city names
  for (int i = 0; i < cache_count; i++) {
    free(cache_cities[i].name);
  }

  // Free the array
  free(cache_cities);

  // Reset state
  cache_cities = NULL;
  cache_count = 0;
  cache_capacity = 0;
}
