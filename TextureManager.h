#pragma once
#include <SDL.h>
#include "Game.hpp"

// Utility class to handle texture loading and rendering
class TextureManager {
public:
    // Load a texture from file path
    static SDL_Texture* loadTexture(const char* filePath);
    
    // Draw a texture with given source and destination rectangles
    static void draw(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect);
    
    // Draw a specific frame from a spritesheet
    static void drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y);
    
    // Clean up all textures
    static void cleanUp();
};
