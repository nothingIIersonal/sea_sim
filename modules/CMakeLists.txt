add_library(ship_spawner_module SHARED
    ship_spawner_module/src/ship_spawner_module.cpp
)
target_link_libraries(ship_spawner_module interconnect object_showcase)

add_library(ship_info_out_module SHARED
    ship_info_out_module/src/ship_info_out_module.cpp
)
target_link_libraries(ship_info_out_module interconnect object_showcase)

add_library(render_module SHARED
    render_module/src/render_module.cpp
)
target_link_libraries(render_module interconnect object_showcase)

add_library(move_module SHARED
    move_module/src/move_module.cpp
)
target_link_libraries(move_module interconnect object_showcase)

add_library(mouse_controller_module SHARED
    mouse_controller_module/src/mouse_controller_module.cpp
)
target_link_libraries(mouse_controller_module interconnect object_showcase)