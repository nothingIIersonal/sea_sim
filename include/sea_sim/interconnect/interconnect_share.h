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


// auto ship_storage_ptr       = std::make_shared<std::map<std::string, Ship>>(ship_storage);
// auto ship_storage_mutex_ptr = std::make_shared<std::shared_mutex>(ship_storage_mutex);
// auto isle_storage_ptr       = std::make_shared<std::map<std::string, Isle>>(isle_storage);
// auto isle_storage_mutex_ptr = std::make_shared<std::shared_mutex>(isle_storage_mutex);


#define SHARED_IC_OBJECTS shared_ic_objects{ ship_storage, ship_storage_mutex, isle_storage, isle_storage_mutex }


#endif