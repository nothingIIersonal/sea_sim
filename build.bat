@echo off

set print_help=false

if "%1%" == "--help" (set print_help=true)
if "%1%" == "-h" (set print_help=true)
if "%1%" == "" (set print_help=true)

if "%print_help%" == "true" (
    echo.
    echo "Usage: .\build.bat <path_to_vcpkg.cmake> <max cpu count>"
    echo "*<path_to_vcpkg.cmake> ---> <your_vcpkg_installation_path>\scripts\buildsystems\vcpkg.cmake"
    echo.
    echo "After success building you can found the out file .\sea_sim.exe in .\.build\Release folder"
    echo.
    exit /B
)

if 1%2 NEQ +1%2 (
    vcpkg install imgui-sfml:x64-windows && vcpkg install imgui[docking-experimental]:x64-windows --recurse && rd /s /q .build & mkdir .build && cd .build && cmake .. -DCMAKE_TOOLCHAIN_FILE=%1 && MSBuild.exe .\sea_sim.sln -p:Configuration=Release /p:Platform=x64
) else (
    vcpkg install imgui-sfml:x64-windows && vcpkg install imgui[docking-experimental]:x64-windows --recurse && rd /s /q .build & mkdir .build && cd .build && cmake .. -DCMAKE_TOOLCHAIN_FILE=%1 && MSBuild.exe .\sea_sim.sln -p:Configuration=Release /p:Platform=x64 -maxcpucount:%2
)