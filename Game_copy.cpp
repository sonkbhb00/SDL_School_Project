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

SDL_Renderer* Game::renderer = nullptr;

// Define static const members outside the class definition
const int Game::MAP_ROWS;
const int Game::MAP_COLS;
const int Game::TILE_SIZE;

Game::Game() : window(nullptr), isRunning(false), player(nullptr), 
               enemySpawnCount(0),
               tileMap(nullptr),
               backgroundTexture(nullptr),
               foregroundTexture(nullptr),
               closestTexture(nullptr),
               cameraX(0), cameraY(0), // Initialize camera coordinates
               lockCamera(true) // Lock camera by default
{ }

Game::~Game() {
    // Clean up enemies
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

// Helper function to spawn a new enemy
void Game::spawnEnemy(int x, int y) {
    Enemy* newEnemy = new Enemy("assets/Idle.png", "assets/Run.png", 
                     "assets/Take Hit.png", "assets/Death.png",
                     x, y, 1.0f);
    if (newEnemy) {
        enemies.push_back(newEnemy);
        std::cout << "New enemy spawned at (" << x << "," << y << ")" << std::endl;
        enemySpawnCount++;
    } else {
        std::cerr << "Failed to create new enemy!" << std::endl;
    }
}

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
        player = new GameObject("assets/Idle.png", "assets/Run.png", "assets/Jump.png", "assets/Attack.png", 50, 50, 1.0f);
        if (player == nullptr) { 
            std::cerr << "Failed to create player object!" << std::endl;
            isRunning = false; 
            return; 
        }

        // Spawn the first enemy
        spawnEnemy(250, 50);

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
                player->attack();
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                player->parry();
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
    // Get map data safely
    const int (*mapData)[Game::MAP_COLS] = nullptr;
    if (tileMap) {
        mapData = tileMap->getMapMatrix();
    }
    
    // Safely handle player updates
    if (player) {
        // Store player position before physics updates
        int prevPlayerX = player->getX();
        int prevPlayerY = player->getY();
        
        // Store player position for collision detection
        player->prevX = prevPlayerX;
        player->prevY = prevPlayerY;
        
        // Apply physics using static class
        Physics::applyGravity(player);
        Physics::applyFriction(player);
        
        // Check collisions only if map data is available
        if (mapData) {
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
                         player->setY(prevPlayerY + 1); // Nudge down slightly
                         break;
                     case BOTTOM: 
                         player->velocityY = 0; 
                         player->onGround = true;
                         player->setY(prevPlayerY); // Ensure player stays on top
                         break;
                     case LEFT: 
                         player->velocityX = 0; 
                         player->setX(prevPlayerX - 1); // Ensure player doesn't pass through left side
                         break;
                     case RIGHT: 
                         player->velocityX = 0; 
                         player->setX(prevPlayerX + 1); // Ensure player doesn't pass through right side
                         break;
                     default: 
                         // If collision side can't be determined, revert to previous position
                         player->revertPosition();
                         break;
                 }
            } else {
                 // Check ground status if no collision
                 player->onGround = Physics::isOnGround(player, mapData,
                                         Game::MAP_ROWS,
                                         Game::TILE_SIZE, Game::TILE_SIZE);
            }
        }
        
        // If camera is locked, keep the player within screen boundaries
        if (lockCamera) {
            // Calculate visible screen boundaries in world coordinates
            int leftBoundary = cameraX;
            int rightBoundary = cameraX + SCREEN_WIDTH - player->destRect.w;
            int topBoundary = cameraY;
            int bottomBoundary = cameraY + SCREEN_HEIGHT - player->destRect.h;
            
            // Constrain player position to screen boundaries
            if (player->getX() < leftBoundary) {
                player->setX(leftBoundary);
                player->velocityX = 0;
            }
            else if (player->getX() > rightBoundary) {
                player->setX(rightBoundary);
                player->velocityX = 0;
            }
            
            if (player->getY() < topBoundary) {
                player->setY(topBoundary);
                player->velocityY = 0;
            }
            else if (player->getY() > bottomBoundary) {
                player->setY(bottomBoundary);
                player->velocityY = 0;
                player->onGround = true; // Player is on ground if at bottom of screen
            }
        }
        else {
            // Only update camera if not locked
            // Update camera position (center camera on player)
            cameraX = player->getX() + player->getCollider().w / 2 - SCREEN_WIDTH / 2;
            cameraY = player->getY() + player->getCollider().h / 2 - SCREEN_HEIGHT / 2;
            
            // Clamp camera to level bounds
            cameraX = std::max(0, std::min(cameraX, Game::MAP_COLS * Game::TILE_SIZE - SCREEN_WIDTH));
            cameraY = std::max(0, std::min(cameraY, Game::MAP_ROWS * Game::TILE_SIZE - SCREEN_HEIGHT));
        }

        // Update player animation state
        player->update();
    }
    
    // Update enemies - now we loop through all enemies
    if (player && mapData) {
        // Create a vector to track newly dead enemies
        std::vector<size_t> deadEnemyIndices;
        
        // Update each enemy
        for (size_t i = 0; i < enemies.size(); i++) {
            Enemy* enemy = enemies[i];
            if (!enemy) continue; // Skip null pointers
            
            // Store previous position for collision response
            enemy->prevX = enemy->getX();
            enemy->prevY = enemy->getY();

            // Apply physics to enemy - with null checks
            try {
                // --- Physics Calculations ---
                Physics::applyGravity(enemy);
                Physics::applyFriction(enemy);

                // --- Collision Detection & Response ---
                int collisionSide = 0;
                if (Physics::checkCollisionSide(enemy, mapData, Game::MAP_ROWS, Game::TILE_SIZE, Game::TILE_SIZE, &collisionSide)) {
                    // Apply player-like collision response
                    switch (collisionSide) {
                        case TOP:
                            enemy->velocityY = 0;
                            enemy->setY(enemy->prevY + 1); // Nudge down slightly
                            break;
                        case BOTTOM:
                            enemy->velocityY = 0;
                            enemy->onGround = true;
                            enemy->setY(enemy->prevY); // Ensure enemy stays on top
                            break;
                        case LEFT:
                            enemy->velocityX = 0;
                            enemy->setX(enemy->prevX - 1); // Prevent passing through left side
                            break;
                        case RIGHT:
                            enemy->velocityX = 0;
                            enemy->setX(enemy->prevX + 1); // Prevent passing through right side
                            break;
                        default:
                            enemy->revertPosition(); // Fallback
                            break;
                    }
                } else {
                    // Check ground status if no direct collision
                    enemy->onGround = Physics::isOnGround(enemy, mapData, Game::MAP_ROWS, Game::TILE_SIZE, Game::TILE_SIZE);
                }

                // --- AI Movement Decision (Only if on ground AND not permanently disabled) ---
                if (enemy->onGround && !enemy->isPermanentlyDisabled) {
                    int playerX = player->getX();
                    int enemyX = enemy->getX();
                    int detectionRange = 300; // pixels

                    if (std::abs(enemyX - playerX) < detectionRange) {
                        if (!enemy->isTakingHit()) {
                            if (playerX < enemyX) {
                                enemy->aiMoveTowards(enemyX - 3);
                            } else if (playerX > enemyX) {
                                enemy->aiMoveTowards(enemyX + 3);
                            }
                        }
                    } else {
                        enemy->setState(ENEMY_IDLE);
                        enemy->velocityX = 0; // Stop if player out of range
                    }
                } else if (enemy->isPermanentlyDisabled) {
                     // If disabled, ensure velocity is zero
                     enemy->velocityX = 0.0f;
                     
                     // Check if enemy is in DEATH state and animation has completed
                     if (enemy->isDeathAnimComplete()) {
                         // Mark this enemy for spawning new enemies
                         deadEnemyIndices.push_back(i);
                         std::cout << "Enemy " << i << " marked for replacement. Death animation complete." << std::endl;
                     }
                }

                // --- Apply Final Velocity to Position ---
                // Update position based on velocity *after* physics and collision resolution
                enemy->setX(enemy->getX() + static_cast<int>(enemy->velocityX));
                enemy->setY(enemy->getY() + static_cast<int>(enemy->velocityY));

            } catch (const std::exception& e) {
                std::cerr << "Exception during enemy physics: " << e.what() << std::endl;
            }

            // Update enemy animation state based on final physics results
            enemy->update(mapData);
        }
        
        // Handle dead enemies and spawn replacements
        for (auto it = deadEnemyIndices.rbegin(); it != deadEnemyIndices.rend(); ++it) {
            size_t index = *it;
            
            // Only spawn new enemies if we've killed the first enemy and spawned fewer than 2 more
            if (enemySpawnCount <= 3) {
                // Get the position of the dead enemy
                int deadX = enemies[index]->getX();
                int deadY = enemies[index]->getY();
                
                std::cout << "Enemy at index " << index << " died. Spawning replacements. Current spawn count: " << enemySpawnCount << std::endl;
                
                // Spawn 1-2 new enemies at offsets from this position
                spawnEnemy(deadX - 50, deadY);
                
                if (enemySpawnCount < 3) {
                    spawnEnemy(deadX + 50, deadY);
                }
            }
            
            // Remove the dead enemy
            delete enemies[index];
            enemies.erase(enemies.begin() + index);
        }
    }
    
    // ---- ATTACK HIT DETECTION (integrated directly) ----
    // Only check for hits if there's a player and player is attacking
    if (player && player->getState() == ATTACKING) {
        SDL_Rect playerAttackBox = player->getAttackHitbox();

        // Check all enemies for hit detection
        for (auto enemy : enemies) {
            if (!enemy || enemy->isTakingHit() || enemy->isPermanentlyDisabled) {
                continue; // Skip if already hit or dead
            }
            
            SDL_Rect enemyCollider = enemy->getCollider();

            // Offset hitboxes by camera position if camera is not locked
            if (!lockCamera) {
                playerAttackBox.x -= cameraX;
                playerAttackBox.y -= cameraY;
                enemyCollider.x -= cameraX;
                enemyCollider.y -= cameraY;
            }

            if (SDL_HasIntersection(&playerAttackBox, &enemyCollider)) {
                enemy->takeHit();
            }
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    
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
    
    // Render tilemap if available
    if (tileMap) {
        tileMap->render(cameraX, cameraY);
    }
    
    // Render player if available
    if (player) {
        player->render(player->getX() - cameraX, player->getY() - cameraY);
    }
    
    // Render all enemies
    for (auto enemy : enemies) {
        if (enemy) {
            enemy->render(enemy->getX() - cameraX, enemy->getY() - cameraY);
        }
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
    
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    
    delete player;
    player = nullptr;
    
    // Clean up enemies
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    delete tileMap;
    tileMap = nullptr;
    
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(foregroundTexture);
    SDL_DestroyTexture(closestTexture);
    
    std::cout << "Game cleaned" << std::endl;
}

bool Game::running() {
    return isRunning;
} 