cmake_minimum_required(VERSION 3.15)
project(demo)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add SDL2 packages
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2-2.28.5/x86_64-w64-mingw32/lib/cmake/SDL2")
set(SDL2_image_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_image-2.8.5/x86_64-w64-mingw32/lib/cmake/SDL2_image")
set(SDL2_mixer_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_mixer-2.8.1/x86_64-w64-mingw32/lib/cmake/SDL2_mixer")
set(SDL2_ttf_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_ttf-2.24.0/x86_64-w64-mingw32/lib/cmake/SDL2_ttf")

list(APPEND CMAKE_PREFIX_PATH
    ${SDL2_DIR}
    ${SDL2_image_DIR}
    ${SDL2_mixer_DIR}
    ${SDL2_ttf_DIR}
)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2-2.28.5/x86_64-w64-mingw32/include/SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_image-2.8.5/x86_64-w64-mingw32/include/SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_mixer-2.8.1/x86_64-w64-mingw32/include/SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_ttf-2.24.0/x86_64-w64-mingw32/include/SDL2
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2-2.28.5/x86_64-w64-mingw32/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_image-2.8.5/x86_64-w64-mingw32/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_mixer-2.8.1/x86_64-w64-mingw32/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_ttf-2.24.0/x86_64-w64-mingw32/lib
)

# Add source files
set(SOURCES
    main.cpp
    Game.cpp
    GameObject.cpp
    TextureManager.cpp
    TileMap.cpp
    Combat.cpp
    Physics.cpp
    Enemy.cpp
    AudioManager.cpp
)

# Add header files
set(HEADERS
    main.h
    Game.hpp
    GameObject.h
    TextureManager.h
    TileMap.hpp
    Combat.hpp
    Physics.hpp
    Enemy.h
    AudioManager.h
)

# Create executable with WIN32 subsystem
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)

# Copy DLL files to build directory
file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2-2.28.5/x86_64-w64-mingw32/bin/SDL2.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_image-2.8.5/x86_64-w64-mingw32/bin/SDL2_image.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_mixer-2.8.1/x86_64-w64-mingw32/bin/SDL2_mixer.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/Include/SDL2_ttf-2.24.0/x86_64-w64-mingw32/bin/SDL2_ttf.dll"
    DESTINATION "${CMAKE_BINARY_DIR}"
    DESTINATION "${CMAKE_BINARY_DIR}/Debug"
    DESTINATION "${CMAKE_BINARY_DIR}/Release"
)

# Copy assets, audio, and font directories
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_SOURCE_DIR}/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/font
    DESTINATION ${CMAKE_BINARY_DIR}
)