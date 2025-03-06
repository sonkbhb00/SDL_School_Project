#include "game.hpp"
#include "GameObject.h"
#include <SDL.h>

// Nếu biến player được khai báo trong game.cpp, bạn cần khai báo extern ở đây:
extern GameObject* player;

int main(int argc, char* argv[]){
    Game* game = new Game();
    game->init("Shadow of Words", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720);

    Uint32 frameStart;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    int frameTime;

    while(game->running()){
        frameStart = SDL_GetTicks();

        // Xử lý input từ bàn phím
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                // Giả sử game có phương thức dừng; nếu không, bạn có thể dùng exit(0)
                // game->stop(); hoặc thiết lập một biến cờ để thoát vòng lặp
                break;
            }
            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                case SDLK_w:
                    player->Move(0, -10);    // Di chuyển lên khi nhấn 'w'
                    break;
                case SDLK_s:
                    player->Move(0, 10);     // Di chuyển xuống khi nhấn 's'
                    break;
                case SDLK_a:
                    player->Move(-10, 0);    // Di chuyển sang trái khi nhấn 'a'
                    break;
                case SDLK_d:
                player->Move(10, 0);     // Di chuyển sang phải khi nhấn 'd'
                    break;
                default:
                    break;
    }
}

        }

        game->update();
        game->render();

        frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }

    game->clean();
    delete game;
    return 0;
}
