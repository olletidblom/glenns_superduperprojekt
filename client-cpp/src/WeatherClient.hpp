#pragma once
#include <string>



class HTTPClient;
class GEO;
class Meteo;
class Input;


class WeatherClient{
private:
HTTPClient& client;


public:
WeatherClient(HTTPClient& _Client) : client(_Client)
{

}

std::string get_cords(const std::string& city_name);

std::string get_weather(const std::pair<std::string, std::string>& cords);




};