#ifndef INTERCONNECT_SHARE
#define INTERCONNECT_SHARE


#include <cstring>
#include <map>
#include <shared_mutex>


#include <sea_sim/interconnect/interconnect.h>


std::map<std::string, Ship> ship_storage       = {};
std::shared_mutex           ship_storage_mutex = {};
std::map<std::string, Isle> isle_storage       = {};
std::shared_mutex           isle_storage_mutex = {};


auto shared_ic_objects = shared_ic_objects_t{ ship_storage, ship_storage_mutex, isle_storage, isle_storage_mutex };

auto environment = environment_t{};


#endif