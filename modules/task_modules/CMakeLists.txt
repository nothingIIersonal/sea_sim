add_library(ship_router_module SHARED
    ship_router_module/src/ship_router_module.cpp
)
target_link_libraries(ship_router_module interconnect object_showcase)