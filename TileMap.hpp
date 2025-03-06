#pragma once
#include <SDL.h>
#include "Game.hpp"  // Để dùng các hằng số Game::MAP_ROWS, Game::MAP_COLS, Game::TILE_SIZE

class TileMap {
public:
    TileMap();
    ~TileMap();

    void drawMap();
    int (*getMapMatrix())[Game::MAP_COLS] { return grid; }

private:
    SDL_Texture* dirtTexture;
    SDL_Texture* grassBlockTexture;
    SDL_Rect srcRect, destRect;
    int grid[Game::MAP_ROWS][Game::MAP_COLS];
    void loadMap(int arr[Game::MAP_ROWS][Game::MAP_COLS]);
};
