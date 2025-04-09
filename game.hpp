#pragma once
#include <SDL.h>

// Forward declarations
class GameObject;
class TileMap;

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
    static const int MAP_ROWS = 30;
    static const int MAP_COLS = 50;
    static const int TILE_SIZE = 32;

    // Renderer dùng chung
    static SDL_Renderer* renderer;

private:
    SDL_Window* window;
    bool isRunning;
    GameObject* player;
    TileMap* tileMap;
};
