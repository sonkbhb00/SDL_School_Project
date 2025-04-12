
#include "Game.hpp"
#include <SDL.h>

int main(int argc, char* argv[]) {
    Game* game = new Game();
    game->init("Shadow of Words", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 576);

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (game->running()) {
        frameStart = SDL_GetTicks();
        game->handleEvents();
        game->update();
        game->render();
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
            SDL_Delay(frameDelay - frameTime);
    }

    game->clean();
    delete game;
    return 0;
}
