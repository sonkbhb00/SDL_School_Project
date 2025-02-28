#pragma once
#include <SDL.h>
#include "textureManager.h"

class map {
public:
    map();
    void loadMap(int arr[30][50]);

    void drawMap();
private:
    int grid[30][50];
    SDL_Texture* dirt;
    SDL_Texture* grassBlock;
    SDL_Texture* grass;
    SDL_Rect srcRect, destRect;
};
