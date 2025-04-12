$content = @"
#pragma once
#include <SDL.h>
#include <vector>

// Forward declarations
class GameObject;
class TileMap;
class Enemy;

class Game {
public:
    Game();
    ~Game();

    // Khởi tạo game
    void init(const char* title, int xPos, int yPos, int width, int height);

    // Xử lý sự kiện, cập nhật trạng thái và render
    void handleEvents();
    void update();
    void render();
    void clean();

    // Trả về trạng thái game (true nếu game đang chạy)
    bool running();

    // Các hằng số cho map
    static const int MAP_ROWS = 64;
    static const int MAP_COLS = 64;
    static const int TILE_SIZE = 16;
    
    // Screen constants
    static const int SCREEN_WIDTH = 720;
    static const int SCREEN_HEIGHT = 576;

    // Renderer dùng chung
    static SDL_Renderer* renderer;

private:
    // Helper function to spawn a new enemy
    void spawnEnemy(int x, int y);
    
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    std::vector<Enemy*> enemies;
    int enemySpawnCount;
    TileMap* tileMap;
    SDL_Texture* backgroundTexture; // Furthest background (BG1)
    SDL_Texture* foregroundTexture; // Closer background (BG2)
    SDL_Texture* closestTexture;    // Closest background (BG3)
    int cameraX, cameraY;           // Camera position variables
    bool lockCamera;                // Flag to lock camera and constrain player
};
"@

$content | Out-File -FilePath "Game.hpp" -Encoding utf8 