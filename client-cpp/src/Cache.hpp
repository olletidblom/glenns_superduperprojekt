#pragma once
#include <filesystem>
#include <string>



class Cache{
private:
bool dir_created;
std::string dir_path;

public:

Cache(const std::string& path) : dir_path(path)
{
    dir_created = std::filesystem::create_directory(dir_path);
}

int cache_save(std::string& data, std::string name);

std::string cache_load(std::string name, bool check_age);



};