#pragma once
#include <string>



class Meteo{
private:
std::pair<std::string, std::string> cords;

public:
Meteo(const std::pair<std::string, std::string>& _Cords) : cords(_Cords)
{

}

std::string format_request();


};