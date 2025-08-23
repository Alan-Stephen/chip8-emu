cmake_minimum_required(VERSION 4.0)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)

add_executable(app main.cpp
        main.cpp
        AddressRegister.cpp
        AddressRegister.h
        Input.cpp
        Input.h
        Display.cpp
        Display.h)

target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(app PRIVATE ${SDL2_LIBRARIES})
