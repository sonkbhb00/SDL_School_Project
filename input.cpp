
#include "game.hpp"
#include <SDL.h>

// Giả sử rằng biến player được khai báo ở một nơi khác (ví dụ: trong game.cpp)
extent GameObject *player;

// Hàm xử lý input từ bàn phím và di chuyển đối tượng
void processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            // Giả sử bạn có cách để dừng game, ví dụ:
            // game->stop();
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    // Di chuyển đối tượng lên trên 10 đơn vị
                    player->move(0, -10);
                    break;
                case SDLK_DOWN:
                    // Di chuyển đối tượng xuống dưới 10 đơn vị
                    player->move(0, 10);
                    break;
                case SDLK_LEFT:
                    // Di chuyển đối tượng sang trái 10 đơn vị
                    player->move(-10, 0);
                    break;
                case SDLK_RIGHT:
                    // Di chuyển đối tượng sang phải 10 đơn vị
                    player->move(10, 0);
                    break;
                default:
                    break;
            }
        }
    }
}
