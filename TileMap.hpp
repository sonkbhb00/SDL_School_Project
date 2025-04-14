#pragma once
#include <SDL.h>
#include "Game.hpp"  // Still need for other constants like TILE_SIZE

// class Game; // Remove forward declaration

class TileMap {
public:
    // Original constructor
    TileMap();
    ~TileMap();

    // Original signatures
    void drawMap();
    void drawMap(int cameraX, int cameraY);
    void render(int cameraX, int cameraY) { drawMap(cameraX, cameraY); } 
    // Original getter using hardcoded constants
    int (*getMapMatrix())[64] { return grid; }

private:
    SDL_Texture* tileSetTexture;
    SDL_Texture* decorsTexture;
    SDL_Rect srcRect, destRect;
    // Use static grid with hardcoded dimensions
    static int grid[64][64];
    // Original loadMap signature with hardcoded dimensions
    void loadMap(int arr[64][64]);
    // Remove dynamic allocation helpers
    // void allocateGrid();
    // void deallocateGrid();
};
