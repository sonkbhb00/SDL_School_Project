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

    // Screen dimensions - no longer const since they can change
    static int SCREEN_WIDTH;
    static int SCREEN_HEIGHT;

    // Renderer dung chung
    static SDL_Renderer* renderer;

    void spawnRandomEnemy();
    void startFadeEffect() {
        isFading = true;
        fadeStartTime = SDL_GetTicks();
        fadeAlpha = 0;
    }

    void restart();  // Add restart function

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
    //static const Uint32 TIMER_DURATION = 142000; // 2:22 in milliseconds (142 seconds)
    static const Uint32 TIMER_DURATION = 14200; // 2:22 in milliseconds (142 seconds)
    bool hasStartedTimer;  // Track if timer has been started after 10 defeats

    // Mastery animation properties
    struct MasteryFrame {
        int x, y, w, h;
    };
    static const MasteryFrame MASTERY_FRAMES[30];
    SDL_Texture* masteryTexture;
    bool showMasteryAnimation;
    Uint32 masteryStartTime;
    int masteryFrame;
    static const int MASTERY_TOTAL_FRAMES = 30;
    static const Uint32 MASTERY_FRAME_DURATION = 100;  // 3000ms total / 30 frames = 100ms per frame

    // Screen fade properties
    bool isFading;
    Uint32 fadeStartTime;
    static const Uint32 FADE_DURATION = 3000; // Changed from 8000 to 3000 (3 seconds)
    Uint8 fadeAlpha;  // Alpha value for fade overlay

    // Death text properties
    bool showDeathText;
    Uint32 deathTextStartTime;
    static const Uint32 DEATH_TEXT_DURATION = 3000; // 3 seconds to reach full brightness
    Uint8 deathTextAlpha;

    bool canRestart;  // Flag to control when restart is allowed
    bool isPaused;
    SDL_Texture* pauseScreenTexture;

    SDL_Texture* instructionTexture;
    bool showInstructions;  // Flag to specifically show instructions

    // Add near other boolean flags
    bool showInitialInstructions;
    bool showEndGameScreen;  // Flag to show end game screen
};
