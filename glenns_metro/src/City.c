#include "City.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "utils/utils.h"

//--------------Internal functions----------------



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

  _City->name = strdup_1(_Name);
  if (_City->name == NULL)
  {
    printf("Failed to allocate memory for City name\n");
    free(_City);
    return -1;
  }

  if (countryCode != NULL)
  {
    _City->countryCode = strdup_1(countryCode);
  }
  else
  {
    _City->countryCode = strdup_1("");
  }

  if (_City->countryCode == NULL)
  {
    printf("Failed to allocate memory for City name\n");
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
