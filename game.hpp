#pragma once
#include <SDL.h>
// Không include "GameObject.h" ở đây để tránh vòng lặp include
// Thay vào đó, dùng forward declaration:
class GameObject;

// Forward declaration cho TileMap cũng dùng để tránh vòng lặp include
class TileMap;

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xPos, int yPos, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() const { return isRunning; }

    // Renderer dùng chung cho toàn bộ game
    static SDL_Renderer* renderer;

    // Các hằng số tích hợp trực tiếp vào Game
    inline static constexpr int TILE_SIZE = 32;
    inline static constexpr int MAP_ROWS = 30;
    inline static constexpr int MAP_COLS = 50;

private:
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    TileMap* tileMap;
};
