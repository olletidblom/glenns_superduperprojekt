#include <format>
#include <iostream>
#include "TCPClient.hpp"

int main()
{
    std::string city_name;
    std::string country_code;
    TCPClient client("kontoret.onvo.se", 10180, -1);

    std::cout << "WeatherClient C++\n\n";

    std::cout << "Input a city: ";
    std::cin >> city_name;

    std::cout << "\nInput country code of city: ";
    std::cin >> country_code;
    
    client.TCPClient_Connect();

    int result = client.TCPClient_Send("GET /api/v1/gwd?city=" + city_name + "&countryCode=" + country_code + " HTTP/1.1\r\nHost: kontoret.onvo.se\r\nConnection: close\r\n\r\n");

    if (result != 0)
    {
        return -1;
    }

    std::string response = client.TCPClient_Recieve();

    while (response.empty())
        continue;

    printf("\nResponse: %s \n", response.c_str());

    return 0;
}