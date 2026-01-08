#include "GEO.hpp"





std::string GEO::format_request()
{
    std::string request = ("localhost:10180/api/v1/geo?city=" + city_name);
    return request;
}