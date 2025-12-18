#include "Input.hpp"
#include <iostream>

int Input::get_input_option()
{
    printf("Select an option\n");
    printf("Get Co-ordinates [1]\n");
    printf("Get Weather [2]\n");
    printf("Exit [3]\n");
    std::string selected;
    std::getline(std::cin, selected);
    
    if(selected == "1")
    selected_option = 1;
    else if(selected == "2")
    selected_option = 2;
    else if(selected == "3")
    selected_option = 3;
    else
    selected_option = -1;

    return selected_option;
}

std::string Input::get_input_city()
{
    printf("Input a city: ");

    std::getline(std::cin, city_name);

    return city_name;
}

std::pair<std::string, std::string> Input::get_input_coords()
{
    printf("Input latitude: \n");
    std::getline(std::cin, lat);
    printf("Input longitude: \n");
    std::getline(std::cin, lon);

    std::pair<std::string, std::string> cords(lat, lon);
    return cords;
}