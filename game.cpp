#include "game.hpp"
#include "textureManager.h"
SDL_Texture* playerTexture;
SDL_Rect* srcR, destR;


Game::Game(){}
Game::~Game(){};


void Game::init(const char *title, int xPos, int yPos, int width, int height){
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0){
        cout << "subsys" << endl;
        window = SDL_CreateWindow(title, xPos, yPos, width,height, SDL_WINDOW_SHOWN);
        if(window){
            cout << "window created" << endl;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer){
            isRunning = true;
        }
        else{
            isRunning = false;

        }
    playerTexture = textureManeger::loadTexture("assets//player_m_1.png",renderer);
    }
}

void Game::handleEvents(){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}
void Game::update(){
    cnt++;
    destR.h = 86;
    destR.w = 32;
    destR.x = cnt;
    destR.y = 50;
    cout << cnt << endl;

}

void Game::render(){
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,playerTexture, NULL, &destR);
    SDL_RenderPresent(renderer);
}

void Game::clean(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_QUIT;
    cout << "game cleaned" << endl;
}
