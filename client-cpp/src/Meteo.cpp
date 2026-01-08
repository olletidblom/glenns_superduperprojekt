#include "Meteo.hpp"




std::string Meteo::format_request()
{
    std::string request = ("localhost:10180/api/v1/gwd?lat=" + cords.first + "&lon=" + cords.second);
    return request;
}