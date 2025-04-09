#include "Game.hpp"
#include "TextureManager.h"
#include "Physics.hpp"
#include "TileMap.hpp"
#include "Combat.hpp"
#include <SDL_image.h>
#include <iostream>

using namespace std;

SDL_Renderer* Game::renderer = nullptr;

Game::Game() : window(nullptr), isRunning(false), player(nullptr), tileMap(nullptr) { }

Game::~Game() { }

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
        // Khởi tạo player với sprite sheet có 2 hàng: row 0 cho idle và row 1 cho chạy
        player = new GameObject("assets/Idle.png", 50, 50, 1.0f, 11, 1, 150);
        tileMap = new TileMap();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            isRunning = false;
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
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                Combat::attack(player);
            }
        }
    }
}

void Game::update() {
    Physics::applyGravity(player);
    if (Physics::checkCollisionWithMap(player, tileMap->getMapMatrix(),
                                       Game::MAP_ROWS, Game::MAP_COLS,
                                       Game::TILE_SIZE, Game::TILE_SIZE)) {
        player->revertPosition();
    }
    player->update();
    // Kiểm tra thời gian attack để phục hồi animation ban đầu
    if (player->isAttacking) {
    Uint32 now = SDL_GetTicks();
    if (now - player->attackStartTime >= player->attackDuration) {
        // Lưu texture tấn công để có thể giải phóng sau
        SDL_Texture* attackTexture = player->getTexture();

        // Khôi phục texture gốc
        player->setTexture(player->originalTexture);
        player->setTotalFrames(player->originalTotalFrames);
        player->setTotalRows(player->originalTotalRows);
        player->setAnimSpeed(player->originalAnimSpeed);
        player->resetAnimation();
        player->isAttacking = false;

        // Giải phóng texture tấn công nếu khác với texture gốc
        if (attackTexture != player->originalTexture) {
            SDL_DestroyTexture(attackTexture);
        }

        cout << "Attack animation finished. Restoring normal animation." << endl;
    }
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

bool Game::running() {
    return isRunning;
}
