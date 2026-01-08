#include "Cache.hpp"
#include <iostream>
#include <fstream>

int Cache::cache_save(std::string &data, std::string name)
{
    std::ofstream file("cache/" + name + ".json");
    
    if(file.is_open())
    {
        file << data;
        file.close();
    }
    return 0;
}

std::string Cache::cache_load(std::string name, bool check_age)
{
    std::filesystem::path path = "cache/" + name + ".json";
    std::string cached_data = {};
    if (std::filesystem::exists(path))
    {
        if (check_age)
        {
            auto wtime = std::filesystem::last_write_time(path);
            auto time_now = std::filesystem::file_time_type::clock::now();

            auto age = time_now - wtime;

            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(age).count();
            if (seconds >= 900)
            {
                return {};
            }
        }

        std::ifstream file("cache/" + name + ".json");
        std::string line;
        if(file.is_open())
        {
            while(std::getline(file, line))
            {
                cached_data.append(line);
                cached_data.append("\n");
            }
            file.close();
            return cached_data;
        }
    }

    return {};
}