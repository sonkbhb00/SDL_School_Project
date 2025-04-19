#include <SDL.h>
#include "Game.hpp"

#undef main  // Undefine main if it was defined by SDL

/**
 * Main entry point for the Knight Fight game
 * Initializes the game, manages the main game loop and frame timing
 */
int main(int argc, char* argv[]) {
    // Frame rate control
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    // Initialize game
    Game* game = new Game();
    game->init("Knight fight", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 576);

    // Main game loop
    while (game->running()) {
        frameStart = SDL_GetTicks();

        // Game cycle: process input, update state, render
        game->handleEvents();
        game->update();
        game->render();

        // Cap frame rate
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // Clean up resources
    game->clean();
    delete game;
    return 0;
}
