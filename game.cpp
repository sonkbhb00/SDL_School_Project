#include "Game.hpp"
#include "GameObject.h"
#include "TextureManager.h"
#include "TileMap.hpp"
#include "Combat.hpp"
#include "Physics.hpp"
#include <SDL_image.h>
#include <iostream>
#include <algorithm> // For std::min/max

using namespace std;

SDL_Renderer* Game::renderer = nullptr;

// Define static const members outside the class definition
const int Game::MAP_ROWS;
const int Game::MAP_COLS;
const int Game::TILE_SIZE;

Game::Game() : window(nullptr), isRunning(false), player(nullptr), tileMap(nullptr),
               backgroundTexture(nullptr),
               foregroundTexture(nullptr),
               closestTexture(nullptr),
               cameraX(0), cameraY(0) // Initialize camera coordinates
{ }

Game::~Game() { }

void Game::init(const char* title, int xPos, int yPos, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, xPos, yPos, width, height, SDL_WINDOW_SHOWN);
        if (window) { cout << "Window created" << endl; }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) { isRunning = true; } else { isRunning = false; return; }
        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { cout << "IMG_Init failed: " << IMG_GetError() << endl; isRunning = false; return; }

        // Load background (BG1)
        backgroundTexture = TextureManager::loadTexture("assets/BG1.png");
        if (backgroundTexture == nullptr) {
            cout << "Failed to load background: assets/BG1.png" << endl;
        }

        // Load foreground (BG2)
        foregroundTexture = TextureManager::loadTexture("assets/BG2.png");
        if (foregroundTexture == nullptr) {
            cout << "Failed to load foreground: assets/BG2.png" << endl;
        }

        // Load closest (BG3)
        closestTexture = TextureManager::loadTexture("assets/BG3.png");
        if (closestTexture == nullptr) {
            cout << "Failed to load closest: assets/BG3.png" << endl;
        }

        // Initialize player
        player = new GameObject("assets/Idle.png", "assets/Run.png", "assets/Jump.png", 50, 50, 1.0f);
        if (player == nullptr) { /* error handling */ isRunning = false; return; }

        // Initialize tilemap
        tileMap = new TileMap();
        if (tileMap == nullptr) { /* error handling */ isRunning = false; return; }

    } else {
        cout << "SDL_Init failed: " << SDL_GetError() << endl;
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
                case SDLK_w:
                case SDLK_SPACE:
                    player->jump();
                    break;
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
            if (event.button.button == SDL_BUTTON_LEFT) {
                Combat::attack(player);
            }
        }
    }
    
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    // Pass only the horizontal component to move()
    if (keyState[SDL_SCANCODE_A]) {
        player->move(-1); // Pass direction/intent
    }
    if (keyState[SDL_SCANCODE_D]) {
        player->move(1); // Pass direction/intent
    }
}

void Game::update() {
    const int (*mapData)[Game::MAP_COLS] = tileMap->getMapMatrix();
    
    // Apply physics using static class
    Physics::applyGravity(player);
    Physics::applyFriction(player); 
    // Note: Position updates might happen within applyGravity/applyFriction now
    // Or might need a separate Physics::updatePosition(player) call

    // Store position *after* potential physics updates
    int playerY = player->getY();

    // Update camera position (center camera on player)
    cameraX = player->getX() + player->getCollider().w / 2 - 720 / 2;
    cameraY = player->getY() + player->getCollider().h / 2 - 576 / 2;
    
    // Clamp camera to level bounds
    cameraX = std::max(0, std::min(cameraX, Game::MAP_COLS * Game::TILE_SIZE - 720));
    cameraY = std::max(0, std::min(cameraY, Game::MAP_ROWS * Game::TILE_SIZE - 576));

    // Check collisions using static class
    int collisionSide = 0;
    if (Physics::checkCollisionSide(player, mapData,
                               Game::MAP_ROWS,
                               Game::TILE_SIZE, Game::TILE_SIZE,
                               &collisionSide)) {
        // Basic collision response - adjust as needed
        switch (collisionSide) {
             case TOP: 
                 player->velocityY = 0; 
                 player->setY(playerY + 1); // Nudge down slightly?
                 break;
             case BOTTOM: 
                 player->velocityY = 0; 
                 player->onGround = true; 
                 // Use previous position for Y correction if needed
                 // player->setY(player->prevY); 
                 break;
             case LEFT: 
             case RIGHT: 
                 player->velocityX = 0; 
                 // Use previous position for X correction if needed
                 // player->setX(player->prevX);
                 break;
             default: 
                 player->revertPosition(); // Fallback?
                 break;
         }
         // Alternative: Just revert position on any collision
         // player->revertPosition(); 
    } else {
         // Check ground status if no collision
         player->onGround = Physics::isOnGround(player, mapData,
                                 Game::MAP_ROWS,
                                 Game::TILE_SIZE, Game::TILE_SIZE);
    }

    // Update player animation state based on physics results
    player->update(); 

    // Check attack state
    if (player->isAttacking) {
        Uint32 now = SDL_GetTicks();
        if (now - player->attackStartTime >= static_cast<Uint32>(player->attackDuration)) {
            player->isAttacking = false;
            // std::cout << "Attack state finished." << std::endl;
        }
    }
}

void Game::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render background images
    if (backgroundTexture) {
        // Background moves slower (parallax effect)
        SDL_Rect bgRect = {-cameraX/4, -cameraY/4, 720, 576};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
    }
    if (foregroundTexture) {
        // Middle background moves at medium speed
        SDL_Rect fgRect = {-cameraX/2, -cameraY/2, 720, 576};
        SDL_RenderCopy(renderer, foregroundTexture, NULL, &fgRect);
    }
    if (closestTexture) {
        // Closest background moves at faster speed (nearly same as tilemap)
        SDL_Rect closeRect = {-cameraX*3/4, -cameraY*3/4, 720, 576};
        SDL_RenderCopy(renderer, closestTexture, NULL, &closeRect);
    }

    // Tell TileMap to render with camera offset
    tileMap->drawMap(cameraX, cameraY);

    // Render player (adjusted for camera)
    SDL_Rect playerDest = player->getCollider(); 
    playerDest.x -= cameraX;
    playerDest.y -= cameraY;
    player->render(playerDest.x, playerDest.y); // We'll need to update render to accept position

    // Present the frame
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    delete player;
    player = nullptr;
    delete tileMap;
    tileMap = nullptr;

    // Destroy textures
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (foregroundTexture) {
        SDL_DestroyTexture(foregroundTexture);
        foregroundTexture = nullptr;
    }
    if (closestTexture) {
        SDL_DestroyTexture(closestTexture);
        closestTexture = nullptr;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    IMG_Quit();
    SDL_Quit();
    std::cout << "Game cleaned" << std::endl;
}

bool Game::running() {
    return isRunning;
}
