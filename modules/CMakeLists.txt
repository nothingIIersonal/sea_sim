add_library(module_1 SHARED
    module_1/src/module_1.cpp
)
target_link_libraries(module_1 interconnect)

add_library(module_2 SHARED
    module_2/src/module_2.cpp
)
target_link_libraries(module_2 interconnect)

add_library(module_3 SHARED
    module_3/src/module_3.cpp
)
target_link_libraries(module_3 interconnect)