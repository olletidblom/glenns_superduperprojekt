#include <format>
#include <iostream>
#include "HTTP.hpp"
#include "Input.hpp"
#include "WeatherClient.hpp"

int main()
{
    HTTPClient http;
    Input input;
    WeatherClient client(http);
    int option = 0;
    std::string response;

    while (option != 3)
    {
        option = input.get_input_option();

        if (option == 1)
        {
            std::string city_name = input.get_input_city();
            response = client.get_cords(city_name);
        }
        else if (option == 2)
        {
            std::pair<std::string, std::string> cords = input.get_input_coords();
            response = client.get_weather(cords);
        }
        else if(option == 3)
        {
            std::cout << "Exiting" << std::endl;
        }
        else
        {
            std::cout << "Invalid option!" << std::endl;
        }

        std::cout << response << std::endl;
    }

    return 0;
}