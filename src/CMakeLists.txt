# Add executable
add_executable(SDLGame
    game.cpp
    game.h
    main.cpp
    map.cpp
    map.h
    player.cpp
    player.h
    enemy.cpp
    enemy.h
    physics.cpp
    physics.h
    texturemanager.cpp
    texturemanager.h
)

# Link SDL2
target_link_libraries(SDLGame PRIVATE SDL2::SDL2 SDL2::SDL2main)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})