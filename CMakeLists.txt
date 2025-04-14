cmake_minimum_required(VERSION 3.10)
project(SDLGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Add the source directory
add_subdirectory(src)