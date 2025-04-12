#include "Game.hpp"
#include "GameObject.h"
#include "TextureManager.h"
#include "TileMap.hpp"
#include "Combat.hpp"
#include "Physics.hpp"
#include "Enemy.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm> // For std::min/max

using namespace std;

// Static member definitions
SDL_Renderer* Game::renderer = nullptr;

Game::Game() : window(nullptr), isRunning(false), player(nullptr),
               enemy(nullptr), // Initialize single enemy pointer
               tileMap(nullptr),
               backgroundTexture(nullptr),
               foregroundTexture(nullptr),
               closestTexture(nullptr),
               cameraX(0), cameraY(0),
               lockCamera(true)
{ }

Game::~Game() {
}

void Game::init(const char* title, int xPos, int yPos, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, xPos, yPos, width, height, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, 0); // Initialize static renderer
        if (!renderer) { isRunning = false; return; }
        isRunning = true;
        IMG_Init(IMG_INIT_PNG);

        // Use original TextureManager calls (assuming it used Game::renderer)
        backgroundTexture = TextureManager::loadTexture("assets/BG1.png");
        foregroundTexture = TextureManager::loadTexture("assets/BG2.png");
        closestTexture = TextureManager::loadTexture("assets/BG3.png");

        // Use original GameObject constructor
        player = new GameObject("assets/Idle.png", "assets/Run.png", "assets/Jump.png", "assets/Attack.png", 50, 50, 1.0f);
        if (!player) { isRunning = false; return; }

        // Use original Enemy constructor
        enemy = new Enemy("assets/Idle.png", "assets/Run.png",
                         "assets/Take Hit.png", "assets/Death.png",
                         250, 50, 1.0f);
        if (!enemy) { isRunning = false; return; }

        // Use original TileMap constructor (assuming it used Game constants)
        tileMap = new TileMap();
        if (!tileMap) { isRunning = false; return; }

    } else {
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            isRunning = false;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w: case SDLK_SPACE: player->jump(); break;
                case SDLK_s:
                    // Removed player->move(0, 5); // Vertical movement handled by physics
                    break;
                // Cases A and D are handled by continuous key presses below
                case SDLK_a: 
                case SDLK_d:
                    break;
                default:
                    break;
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
             if (event.button.button == SDL_BUTTON_LEFT) player->attack();
             else if (event.button.button == SDL_BUTTON_RIGHT) player->parry();
        }
    }
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_A]) player->move(-1);
    if (keyState[SDL_SCANCODE_D]) player->move(1);
}

void Game::update() {
    const int (*mapData)[Game::MAP_COLS] = nullptr; // Use original map data type
    if (tileMap) {
        mapData = tileMap->getMapMatrix();
    }

    if (player) {
        int prevPlayerX = player->getX();
        int prevPlayerY = player->getY();
        player->prevX = prevPlayerX;
        player->prevY = prevPlayerY;
        Physics::applyGravity(player);
        Physics::applyFriction(player);
        int collisionSide = NONE;
        if (mapData) {
            if (Physics::checkCollisionSide(player, mapData, Game::TILE_SIZE, &collisionSide)) {
                if (collisionSide == BOTTOM) {
                    player->velocityY = 0;
                    player->onGround = true;
                    player->setY( ( (player->getY() + player->destRect.h) / Game::TILE_SIZE) * Game::TILE_SIZE - player->destRect.h );
                } else if (collisionSide == TOP) {
                    player->velocityY = 0;
                } else if (collisionSide == LEFT || collisionSide == RIGHT) {
                    player->velocityX = 0;
                    player->revertPosition();
                }
            } else {
                player->onGround = Physics::isOnGround(player, mapData, Game::TILE_SIZE);
            }
        }
        if (lockCamera) {
             int leftBoundary = cameraX;
             int rightBoundary = cameraX + SCREEN_WIDTH - player->destRect.w;
             int topBoundary = cameraY;
             int bottomBoundary = cameraY + SCREEN_HEIGHT - player->destRect.h;
             if (player->getX() < leftBoundary) { player->setX(leftBoundary); player->velocityX = 0; }
             else if (player->getX() > rightBoundary) { player->setX(rightBoundary); player->velocityX = 0; }
             if (player->getY() < topBoundary) { player->setY(topBoundary); player->velocityY = 0; }
             else if (player->getY() > bottomBoundary) { player->setY(bottomBoundary); player->velocityY = 0; player->onGround = true; }
        } else {
            cameraX = player->getX() + player->getCollider().w / 2 - SCREEN_WIDTH / 2;
            cameraY = player->getY() + player->getCollider().h / 2 - SCREEN_HEIGHT / 2;
            cameraX = std::max(0, std::min(cameraX, Game::MAP_COLS * Game::TILE_SIZE - SCREEN_WIDTH));
            cameraY = std::max(0, std::min(cameraY, Game::MAP_ROWS * Game::TILE_SIZE - SCREEN_HEIGHT));
        }
        player->update();
    }

    if (enemy && player && mapData) {
        enemy->prevX = enemy->getX();
        enemy->prevY = enemy->getY();
        Physics::applyGravity(enemy);
        Physics::applyFriction(enemy);
        int enemyCollisionSide = NONE;
        if (Physics::checkCollisionSide(enemy, mapData, Game::TILE_SIZE, &enemyCollisionSide)) {
            if (enemyCollisionSide == BOTTOM) {
                enemy->velocityY = 0;
                enemy->onGround = true;
                enemy->setY( ( (enemy->getY() + enemy->destRect.h) / Game::TILE_SIZE) * Game::TILE_SIZE - enemy->destRect.h );
            } else if (enemyCollisionSide == TOP) {
                enemy->velocityY = 0;
            } else if (enemyCollisionSide == LEFT || enemyCollisionSide == RIGHT) {
                enemy->velocityX = 0;
                enemy->revertPosition();
            }
        } else {
            enemy->onGround = Physics::isOnGround(enemy, mapData, Game::TILE_SIZE);
        }
        if (enemy->onGround && !enemy->isPermanentlyDisabled) {
             int playerX = player->getX();
             int enemyX = enemy->getX();
             int detectionRange = 300;
             if (std::abs(enemyX - playerX) < detectionRange && !enemy->isTakingHit()) {
                 if (playerX < enemyX) enemy->aiMoveTowards(enemyX - 3);
                 else if (playerX > enemyX) enemy->aiMoveTowards(enemyX + 3);
             } else if (!enemy->isTakingHit()) {
                 enemy->setState(ENEMY_IDLE);
                 enemy->velocityX = 0;
             }
        } else if (enemy->isPermanentlyDisabled || enemy->isTakingHit()) {
            enemy->velocityX = 0.0f;
        }
        enemy->update(mapData);
    }

    if (player && enemy && player->getState() == ATTACKING) {
        SDL_Rect playerAttackBox = player->getAttackHitbox();
        SDL_Rect enemyCollider = enemy->getCollider();
        if (!lockCamera) {
            playerAttackBox.x -= cameraX;
            playerAttackBox.y -= cameraY;
            enemyCollider.x -= cameraX;
            enemyCollider.y -= cameraY;
        }
        if (SDL_HasIntersection(&playerAttackBox, &enemyCollider) &&
            !enemy->isTakingHit() && !enemy->isPermanentlyDisabled) {
            enemy->takeHit();
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer); // Uses static renderer
    
    // Render background (furthest layer)
    if (backgroundTexture) {
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
    }
    
    // Render foreground (middle layer) with parallax scrolling
    if (foregroundTexture) {
        // Slower parallax for middle layer (moves at 1/2 speed of the map)
        int parallaxX = cameraX / 2;
        // Create source rect for tiling and wrapping
        SDL_Rect srcRect = {
            parallaxX % SCREEN_WIDTH,
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, foregroundTexture, &srcRect, &destRect);
        
        // Render the wrap-around portion if needed
        if (srcRect.x > 0) {
            SDL_Rect wrapSrcRect = {0, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_Rect wrapDestRect = {SCREEN_WIDTH - srcRect.x, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, foregroundTexture, &wrapSrcRect, &wrapDestRect);
        }
    }
    
    if (player) {
        player->render(player->getX() - cameraX, player->getY() - cameraY); // Use original GameObject render
    }
    
    if (enemy) {
        enemy->render(enemy->getX() - cameraX, enemy->getY() - cameraY); // Use original Enemy render
    }
    
    // Render closest layer (front layer) with faster parallax
    if (closestTexture) {
        // Faster parallax for front layer (moves at 2x speed of the map)
        int parallaxX = cameraX * 2;
        SDL_Rect srcRect = {
            parallaxX % SCREEN_WIDTH,
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, closestTexture, &srcRect, &destRect);
        
        // Render the wrap-around portion if needed
        if (srcRect.x > 0) {
            SDL_Rect wrapSrcRect = {0, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_Rect wrapDestRect = {SCREEN_WIDTH - srcRect.x, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, closestTexture, &wrapSrcRect, &wrapDestRect);
        }
    }
    
    // Render tilemap last (in front of everything)
    if (tileMap) {
        tileMap->render(cameraX, cameraY); // Use original TileMap render
    }
    
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Destroy textures - TextureManager::cleanUp() assumes it handles this
    TextureManager::cleanUp();
    delete player;
    player = nullptr;
    delete enemy;
    enemy = nullptr;
    delete tileMap;
    tileMap = nullptr;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game cleaned" << std::endl;
}

bool Game::running() {
    return isRunning;
} 