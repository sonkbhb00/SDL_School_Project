#pragma once
#include "Game.hpp"

class TextureManager {
public:
    static SDL_Texture* loadTexture(const char* filePath);
    static void draw(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect);
    static void drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y);
};
