#pragma once
#include <string>




class GEO{
private:
std::string city_name;

public:
GEO(const std::string& _CityName) : city_name(_CityName)
{

}

std::string format_request();


};