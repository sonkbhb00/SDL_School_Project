#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <fstream>
#include "AudioManager.h"

// Forward declarations
class GameObject;
class TileMap;
class Enemy;

class Game {
public:
    Game();
    ~Game();

    // Initialize the game
    void init(const char* title, int xPos, int yPos, int width, int height);

    // Handle events, update game state and render
    void handleEvents();
    void update();
    void render();
    void clean();

    // Returns true if game is running
    bool running();

    // Map constants
    static const int MAP_ROWS = 64;
    static const int MAP_COLS = 64;
    static const int TILE_SIZE = 16;

    // Screen dimensions
    static int SCREEN_WIDTH;
    static int SCREEN_HEIGHT;

    // Shared renderer
    static SDL_Renderer* renderer;

    void spawnRandomEnemy();
    void startFadeEffect() {
        isFading = true;
        fadeStartTime = SDL_GetTicks();
        fadeAlpha = 0;
    }

    void restart();

private:
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    std::vector<Enemy*> enemies;
    bool firstWaveDefeated;
    int defeatedEnemyCount;
    TileMap* tileMap;
    SDL_Texture* backgroundTexture;
    SDL_Texture* foregroundTexture;
    SDL_Texture* closestTexture;
    int cameraX, cameraY;
    bool lockCamera;
    bool secondMusicStarted;
    TTF_Font* font;

    // Parry text properties
    bool showParryText;
    Uint32 parryTextStartTime;
    static const Uint32 PARRY_TEXT_DURATION = 1000; // 1 second
    SDL_Color parryTextColor;
    int parryTextSize;
    int successfulParryCount;

    // Timer properties
    bool timerStarted;
    Uint32 timerStartTime;
    static const Uint32 TIMER_DURATION = 142000; // 2:22 in milliseconds
    bool hasStartedTimer;

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
    static const Uint32 MASTERY_FRAME_DURATION = 100;

    // Screen fade properties
    bool isFading;
    Uint32 fadeStartTime;
    static const Uint32 FADE_DURATION = 3000; // 3 seconds
    Uint8 fadeAlpha;

    // Death text properties
    bool showDeathText;
    Uint32 deathTextStartTime;
    static const Uint32 DEATH_TEXT_DURATION = 3000; // 3 seconds
    Uint8 deathTextAlpha;

    bool canRestart;
    bool isPaused;
    SDL_Texture* pauseScreenTexture;

    // Instructions screen properties
    bool showInstructions;
    SDL_Texture* instructionsTexture;
    bool showInitialInstructions;
    bool showEndGameScreen;
};
