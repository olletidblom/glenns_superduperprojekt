#ifndef CITY_CACHE_H
#define CITY_CACHE_H

#include "ui.h"

// Initialize the cache and load from file
// Returns 0 on success, -1 on error
int cache_init(void);

// Add a city to the cache (deduplicates by lat/lon)
// Returns 0 on success, -1 on error
int cache_add_city(const City *city);

// Get all cached cities (sorted alphabetically)
// Returns pointer to array and sets count
// Returns NULL if cache is empty
City *cache_get_all(int *count);

// Save cache to file
// Returns 0 on success, -1 on error
int cache_save(void);

// Free cache memory
void cache_dispose(void);

#endif // CITY_CACHE_H
