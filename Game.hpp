#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "AudioManager.h"

// Forward declarations
class GameObject;
class TileMap;
class Enemy;

class Game {
public:
    Game();
    ~Game();

    // Khoi tao game
    void init(const char* title, int xPos, int yPos, int width, int height);

    // Xu ly su kien, cap nhat trang thai va render
    void handleEvents();
    void update();
    void render();
    void clean();

    // Tra ve trang thai game (true neu game dang chay)
    bool running();

    // Cac hang so cho map
    static const int MAP_ROWS = 64;
    static const int MAP_COLS = 64;
    static const int TILE_SIZE = 16;
    
    // Screen constants
    static const int SCREEN_WIDTH = 720;
    static const int SCREEN_HEIGHT = 576;

    // Renderer dung chung
    static SDL_Renderer* renderer;

private:
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    Enemy* enemy;
    TileMap* tileMap;
    SDL_Texture* backgroundTexture; // Furthest background (BG1)
    SDL_Texture* foregroundTexture; // Closer background (BG2)
    SDL_Texture* closestTexture;    // Closest background (BG3)
    int cameraX, cameraY;           // Camera position variables
    bool lockCamera;                // Flag to lock camera and constrain player
};