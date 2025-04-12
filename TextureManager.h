#pragma once
#include <SDL.h>
// Include Game.hpp again if it uses Game::renderer
#include "Game.hpp"

// Forward declare Game - no longer needed if we pass renderer
// class Game; 

class TextureManager {
public:
    // loadTexture still needs the renderer to create textures
    static SDL_Texture* loadTexture(const char* filePath);
    // draw functions now take the renderer
    static void draw(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect);
    static void drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y);
    // cleanUp doesn't need renderer
    static void cleanUp();
};
