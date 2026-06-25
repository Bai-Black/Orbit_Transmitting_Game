cmake_minimum_required(VERSION 3.16)
project(OrbitTransfer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(orbit_transfer
    src/main.cpp
    src/game.cpp
    src/physics.cpp
    src/orbit.cpp
    src/renderer.cpp
    src/input.cpp
    src/camera.cpp
)

target_include_directories(orbit_transfer PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    src
)

target_link_libraries(orbit_transfer PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
)
