#include "Game.hpp"
#include "TextureManager.h"
#include "Physics.hpp"
#include "TileMap.hpp"
#include <SDL_image.h>
#include <iostream>

using namespace std;

SDL_Renderer* Game::renderer = nullptr;

Game::Game() : window(nullptr), isRunning(false), player(nullptr), tileMap(nullptr) { }

Game::~Game() {
    // Giải phóng tài nguyên được tạo ra (nếu chưa clean)
}

void Game::init(const char* title, int xPos, int yPos, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, xPos, yPos, width, height, SDL_WINDOW_SHOWN);
        if (window) {
            cout << "Window created" << endl;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            isRunning = true;
        } else {
            isRunning = false;
        }
        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            cout << "IMG_Init failed: " << IMG_GetError() << endl;
            isRunning = false;
            return;
        }
        // Khởi tạo đối tượng game
        player = new GameObject("assets/ani.png", 50, 50, 3, 2, 1);
        tileMap = new TileMap(); // TileMap tự load dữ liệu level
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w:
                    player->move(0, -10);
                    break;
                case SDLK_s:
                    player->move(0, 10);
                    break;
                case SDLK_a:
                    player->move(-10, 0);
                    break;
                case SDLK_d:
                    player->move(10, 0);
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::update() {
    player->update();
    // Sử dụng các hằng số từ Game để kiểm tra va chạm
    if (Physics::checkCollisionWithMap(player, tileMap->getMapMatrix(),
                                       Game::MAP_ROWS, Game::MAP_COLS,
                                       Game::TILE_SIZE, Game::TILE_SIZE)) {
        player->revertPosition();
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    tileMap->drawMap();
    player->render();
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout << "Game cleaned" << endl;
    delete player;
    delete tileMap;
}
