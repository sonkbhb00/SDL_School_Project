#include "game.hpp"
#include "textureManeger.h"
#include "GameObject.h"
#include "Physics.hpp"
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

    player = new GameObject("assets\\ani.png", 50, 50, 3, 2 , 1);


    //enemy = new GameObject("assets\\enemy.png", 50, 50);
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
    SDL_RenderPresent(renderer);

}

void Game::update(){
    // Lưu lại vị trí hiện tại của player trước khi cập nhật
    int oldX = player->getX();
    int oldY = player->getY();

    // Cập nhật player
    player->Update();

    // Giả sử bạn có một hàm (hoặc getter) getMapMatrix() trong lớp map để lấy lưới bản đồ
    // Kiểm tra va chạm: nếu player va chạm với tile 0 hoặc 1, revert vị trí
    if(Physics::checkCollisionWithMap(player, Map->getMapMatrix(), 30, 50, 32, 32)) {
        player->revertPosition();
    }
}

void Game::clean(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_QUIT;
    cout << "game cleaned" << endl;
}
