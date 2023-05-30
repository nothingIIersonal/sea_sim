cmake_minimum_required(VERSION 3.5)
project(sea_sim LANGUAGES CXX)


set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_STANDARD 20)


if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/cyrillic.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/icons.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
else()
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/cyrillic.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_CURRENT_LIST_DIR}/misc/icons.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()


include_directories(include)


find_package(Threads REQUIRED)

find_package(imgui CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)


add_library(object_showcase STATIC
    src/sea_sim/toolkit/object_showcase/object_showcase.cpp
)
set_property(TARGET object_showcase PROPERTY POSITION_INDEPENDENT_CODE ON)


add_library(interconnect STATIC
    src/sea_sim/interconnect/interconnect.cpp
)
set_property(TARGET interconnect PROPERTY POSITION_INDEPENDENT_CODE ON)


add_subdirectory(modules)


add_library(GUI STATIC
    src/sea_sim/gui_controller/GUI.cpp
    src/sea_sim/gui_controller/Functions.cpp
    src/sea_sim/gui_controller/RenderEngine.cpp
    src/sea_sim/gui_controller/WindowStorage.cpp
    src/sea_sim/gui_controller/FileDialog.cpp
    src/sea_sim/gui_controller/ModulePageStorage.cpp
    src/sea_sim/gui_controller/ModuleDialog.cpp
    src/sea_sim/gui_controller/GraphicsStorage.cpp
)
target_link_libraries(GUI
    imgui::imgui
    sfml-graphics sfml-system sfml-window
    ImGui-SFML::ImGui-SFML
    object_showcase
)


add_library(thread_pool STATIC
    src/sea_sim/gears/thread_pool.cpp
)


add_executable(${PROJECT_NAME}
    src/sea_sim/core/core.cpp
)
target_link_libraries(${PROJECT_NAME}
    GUI
    thread_pool
    object_showcase
    interconnect
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)
