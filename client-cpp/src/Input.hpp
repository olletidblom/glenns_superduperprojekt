#pragma once
#include <string>
#include <iostream>

class Input{
private:
std::string city_name;
std::string lat;
std::string lon;
int options;
int selected_option;

public:

Input() : options(2)
{

}

int get_input_option();

std::string get_input_city();

std::pair<std::string, std::string> get_input_coords();

};










