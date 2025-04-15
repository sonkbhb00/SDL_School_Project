#include <SDL.h>
#include "Game.hpp"

#undef main  // Undefine main if it was defined by SDL

int main(int argc, char* argv[]) {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    Game* game = new Game();
    game->init("Knight fight!?", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 576);

    while (game->running()) {
        frameStart = SDL_GetTicks();

        game->handleEvents();
        game->update();
        game->render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game->clean();
    delete game;
    return 0;
}
