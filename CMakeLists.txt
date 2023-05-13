cmake_minimum_required(VERSION 3.5)
project(sea_sim LANGUAGES CXX)


set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)


IF (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/4-font.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
ELSE()
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/4-font.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ENDIF()


include_directories(include)

find_package(Threads REQUIRED)

find_package(imgui CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_library(
    interconnect STATIC
    src/sea_sim/interconnect/interconnect.cpp
)
set_property(TARGET interconnect PROPERTY POSITION_INDEPENDENT_CODE ON)


add_library(
    module_1 SHARED
    modules/module_1/src/module_1.cpp
)
target_link_libraries(module_1 interconnect)


add_library(
    module_2 SHARED
    modules/module_2/src/module_2.cpp
)
target_link_libraries(module_2 interconnect)

add_library(
    module_3 SHARED
    modules/module_3/src/module_3.cpp
)
target_link_libraries(module_3 interconnect)


add_executable(${PROJECT_NAME} 
    src/sea_sim/core/core.cpp
    src/sea_sim/gears/thread_pool.cpp
    src/sea_sim/gui_controller/GUI.cpp
    src/sea_sim/gui_controller/functions.cpp
    src/sea_sim/gui_controller/RenderEngine.cpp
    src/sea_sim/gui_controller/window_storage.cpp
    src/sea_sim/gui_controller/FileDialog.cpp
    src/sea_sim/gui_controller/ModulePageStorage.cpp
)

target_link_libraries(${PROJECT_NAME} 
    interconnect
    imgui::imgui
    sfml-graphics sfml-system sfml-window
    ImGui-SFML::ImGui-SFML
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)