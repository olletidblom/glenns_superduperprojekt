#include "WeatherClient.hpp"
#include "HTTP.hpp"
#include "GEO.hpp"
#include "Meteo.hpp"
#include "Input.hpp"
#include "Cache.hpp"

std::string WeatherClient::get_cords(const std::string &city_name)
{
    GEO geo(city_name);
    std::string formatted_request = geo.format_request();

    if (formatted_request.empty())
    {
        return "Failed to build request\n";
    }

    Cache cache("cache");

    std::string cache_name = city_name;

    std::string loaded_cache = cache.cache_load(city_name, 0);

    if (!loaded_cache.empty())
    {
        return loaded_cache;
    }

    std::string response = client.HTTPClient_GET(formatted_request);

    if (response.empty())
    {
        return "Failed to get data\n";
    }

    cache.cache_save(response, cache_name);

    return response;
}

std::string WeatherClient::get_weather(const std::pair<std::string, std::string> &cords)
{
    Meteo meteo(cords);
    std::string formatted_request = meteo.format_request();

    if (formatted_request.empty())
    {
        return "Failed to build request\n";
    }

    Cache cache("cache");

    std::string cache_name = cords.first + cords.second;

    std::string loaded_cache = cache.cache_load(cache_name, 1);

    if (!loaded_cache.empty())
    {
        return loaded_cache;
    }

    std::string response = client.HTTPClient_GET(formatted_request);

    if (response.empty())
    {
        return "Failed to get data\n";
    }

    cache.cache_save(response, cache_name);

    return response;
}