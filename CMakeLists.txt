cmake_minimum_required(VERSION 4.0)https://www.youtube.com/shorts/zQrq9B4KlvA
project(untitled LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED CONFIG)

add_executable(app
    main.cpp
    AddressRegister.cpp
    AddressRegister.h
    Input.cpp
    Input.h
    Display.cpp
    Display.h
    Sprite.cpp
    Sprite.h
)

target_link_libraries(app PRIVATE SDL2::SDL2)

if(NOT TARGET SDL2::SDL2 AND DEFINED SDL2_INCLUDE_DIRS)
    target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

set_target_properties(app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run
    COMMAND $<TARGET_FILE:app>
    DEPENDS app
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
