#include "game.hpp"
#include "textureManager.h"
#include "GameObject.h"
#include "map.hpp"

map *Map;

GameObject *player;
GameObject *enemy;

SDL_Renderer* Game::renderer = nullptr;

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
        if(IMG_Init(IMG_INIT_PNG) == 0) {
            cout << "IMG_Init failed: " << IMG_GetError() << endl;
            isRunning = false;
            return;
        }

    player = new GameObject("assets\\player_m_1.png", 0, 0);
    enemy = new GameObject("assets\\enemy.png", 50, 50);
    Map = new map();
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
    player->Update();
    enemy->Update();

}
void Game::render(){
    // Đặt màu vẽ (background) là trắng
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // (Tùy chọn) Vẽ một hình chữ nhật kiểm tra để debug renderer
    /*SDL_Rect testRect = {100, 100, 50, 50};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &testRect);
    */
    // Render đối tượng GameObject
    Map->drawMap();
    player->Render();
    enemy->Render();
    SDL_RenderPresent(renderer);
}


void Game::clean(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_QUIT;
    cout << "game cleaned" << endl;
}
