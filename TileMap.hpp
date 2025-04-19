#pragma once
#include <SDL.h>
#include "Game.hpp"

// Handles tile map loading and rendering
class TileMap {
public:
    TileMap();
    ~TileMap();

    // Draw the map with camera offset
    void drawMap();
    void drawMap(int cameraX, int cameraY);
    void render(int cameraX, int cameraY) { drawMap(cameraX, cameraY); }
    
    // Access the map data
    int (*getMapMatrix())[64] { return grid; }

private:
    SDL_Texture* tileSetTexture;  // Main tileset image
    SDL_Texture* decorsTexture;   // Decorative elements image
    SDL_Rect srcRect, destRect;   // Source and destination rectangles for rendering
    
    // Map grid with fixed dimensions
    static int grid[64][64];
    
    // Load map data from file
    void loadMap(int arr[64][64]);
};
