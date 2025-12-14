#include "WeatherClient.hpp"
#include "HTTP.hpp"
#include "GEO.hpp"
#include "Meteo.hpp"
#include "Input.hpp"

std::string WeatherClient::get_cords(const std::string &city_name)
{
    GEO geo(city_name);
    std::string formatted_request = geo.format_request();

    if (formatted_request.empty())
    {
        return "Failed to build request\n";
    }

    std::string response = client.HTTPClient_GET(formatted_request);

    if (response.empty())
    {
        return "Failed to get data\n";
    }

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

    std::string response = client.HTTPClient_GET(formatted_request);

    if (response.empty())
    {
        return "Failed to get data\n";
    }
    
    return response;
}