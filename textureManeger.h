#pragma once
#include "game.hpp"

class textureManeger{
public:
    static SDL_Texture* loadTexture(const char* fileName);
    static void draw(SDL_Texture *texture, SDL_Rect srcR, SDL_Rect destR);
    static void drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y);
};
