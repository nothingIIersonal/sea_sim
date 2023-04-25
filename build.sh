#!/bin/bash

if [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ -z "$1" ] ; then
    echo -e "Usage: ./build.sh <path_to_vcpkg.cmake> <make -j number>"
    echo -e "\n*<path_to_vcpkg.cmake> ---> <your_vcpkg_installation_path>/scripts/buildsystems/vcpkg.cmake"
    exit
fi

if [ -z "$2" ]; then
    vcpkg install imgui-sfml:x64-linux &&
    vcpkg install imgui[docking-experimental]:x64-linux --recurse &&
    rm -rf .build &&
    mkdir .build &&
    cd .build &&
    cmake .. -DCMAKE_TOOLCHAIN_FILE=${1} &&
    make
else
    vcpkg install imgui-sfml:x64-linux &&
    vcpkg install imgui[docking-experimental]:x64-linux --recurse &&
    rm -rf .build &&
    mkdir .build &&
    cd .build &&
    cmake .. -DCMAKE_TOOLCHAIN_FILE=${1} &&
    make -j${2}
fi

if [ $? -eq 1 ]; then
    echo -e "\n\n------------------------------------------\nError caused. Fix it and run build again\n------------------------------------------"
else
    echo -e "\n\n------------------------------------------------------\nBuild completed! Out file for run: ./.build/sea_sim\n------------------------------------------------------"
fi