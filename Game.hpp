#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
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

    void spawnRandomEnemy();

private:
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    std::vector<Enemy*> enemies;
    bool firstWaveDefeated;
    int defeatedEnemyCount;  // Add counter for defeated enemies
    TileMap* tileMap;
    SDL_Texture* backgroundTexture;
    SDL_Texture* foregroundTexture;
    SDL_Texture* closestTexture;
    int cameraX, cameraY;
    bool lockCamera;
    bool secondMusicStarted;  // Track if second music has started
    TTF_Font* font;  // Add font member
    
    // Add parry text properties
    bool showParryText;
    Uint32 parryTextStartTime;
    static const Uint32 PARRY_TEXT_DURATION = 1000; // 1 second in milliseconds
    SDL_Color parryTextColor;
    int parryTextSize;
    int successfulParryCount;  // Add counter for successful parries
    
    // Timer properties
    bool timerStarted;
    Uint32 timerStartTime;
    static const Uint32 TIMER_DURATION = 142000; // 2:22 in milliseconds (142 seconds)
    bool hasStartedTimer;  // Track if timer has been started after 10 defeats
};