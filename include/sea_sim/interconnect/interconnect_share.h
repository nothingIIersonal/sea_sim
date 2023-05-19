#include <cstring>
#include <map>
#include <shared_mutex>

#include <sea_sim/interconnect/interconnect.h>


std::map<std::string, Ship> ship_storage = {};
std::shared_mutex ship_storage_mutex = {};
std::map<std::string, Isle> isle_storage = {};
std::shared_mutex isle_storage_mutex = {};


#define SHARE_OBJECTS_PTRS share_objects_ptrs{ std::make_shared<std::map<std::string, Ship>>(ship_storage), \\
                                               std::make_shared<std::shared_mutex>(ship_storage_mutex),     \\
                                               std::make_shared<std::map<std::string, Isle>>(isle_storage), \\
                                               std::make_shared<std::shared_mutex>(isle_storage_mutex) };    