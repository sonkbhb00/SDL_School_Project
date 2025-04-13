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
#include <cmath>

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

        // Initialize audio manager
        if (!AudioManager::getInstance().init()) {
            std::cout << "Failed to initialize AudioManager!" << std::endl;
            isRunning = false;
            return;
        }

        // Play background music
        AudioManager::getInstance().playMusic("audio/medieval-star-188280.mp3");
        AudioManager::getInstance().setMusicVolume(64); // Set to 50% volume

        // Use original TextureManager calls (assuming it used Game::renderer)
        backgroundTexture = TextureManager::loadTexture("assets/BG1.png");
        foregroundTexture = TextureManager::loadTexture("assets/BG2.png");
        closestTexture = TextureManager::loadTexture("assets/BG3.png");

        // Initialize player with size 50x50
        player = new GameObject(50, 50, 50, 50);
        if (player == nullptr) {
            std::cerr << "Failed to create player object!" << std::endl;
            isRunning = false;
            return;
        }

        // Initialize enemy with attack texture
        enemy = new Enemy("assets/Idle.png", "assets/Run.png", "assets/Attack.png", "assets/Take Hit.png", "assets/Death.png", 250, 50, 1.0f);
        if (enemy == nullptr) {
            std::cerr << "Failed to create enemy object!" << std::endl;
            isRunning = false;
            return;
        }

        // Initialize tilemap
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
            int mouseX = event.button.x + cameraX; // Add camera offset to get world coordinates
            int mouseY = event.button.y + cameraY;
            std::cout << "Mouse clicked at: (" << mouseX << ", " << mouseY << ")" << std::endl;

            if (event.button.button == SDL_BUTTON_LEFT) {
                player->attack();
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                player->parry();
            }
        }
    }
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_A]) player->move(-1);
    if (keyState[SDL_SCANCODE_D]) player->move(1);
}

void Game::update() {
    // Update player
    if (player) {
        player->prevX = player->getX();
        player->prevY = player->getY();

        Physics::applyGravity(player);
        Physics::applyFriction(player);

        // Handle camera and player updates
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

    // Update enemy
    if (enemy && player) {
        enemy->prevX = enemy->getX();
        enemy->prevY = enemy->getY();

        // Check distance to player
        float dx = player->getX() - enemy->getX();
        float dy = player->getY() - enemy->getY();
        float distance = std::sqrt(dx * dx + dy * dy);

        // Make the enemy move toward the player or attack based on distance
        if (!enemy->isPermanentlyDisabled && !enemy->isInHitState) {
            if (distance > 65.0f) { // Match the new shorter attack range
                enemy->aiMoveTowards(player->getX());
            } else { // If within attack range, stop and try to attack
                enemy->velocityX = 0.0f;
                enemy->tryAttack(player);
                if (enemy->getState() != ENEMY_ATTACKING) {
                    enemy->setState(ENEMY_IDLE);
                }
            }
        }

        Physics::applyGravity(enemy);
        Physics::applyFriction(enemy);

        // Update enemy with player reference for attack behavior
        enemy->update(player);

        // Handle collisions between enemy attacks and player
        if (enemy->getState() == ENEMY_ATTACKING) {
            SDL_Rect enemyAttackBox = enemy->getAttackHitbox();
            SDL_Rect playerCollider = player->getCollider();

            // Adjust collision boxes for camera if needed
            if (!lockCamera) {
                enemyAttackBox.x -= cameraX;
                enemyAttackBox.y -= cameraY;
                playerCollider.x -= cameraX;
                playerCollider.y -= cameraY;
            }

            // Check if enemy's attack hits player
            if (SDL_HasIntersection(&enemyAttackBox, &playerCollider)) {
                if (player->isParrying) {
                    // Successful parry - move player back
                    float parryKnockback = 10.0f;
                    player->velocityX = player->facingRight ? -parryKnockback : parryKnockback;
                    std::cout << "Attack parried!" << std::endl;
                } else {
                    std::cout << "Player hit by enemy!" << std::endl;
                    player->takeHit();
                }
            }
        }
    }

    // Handle player's attack hitting enemy
    if (player && enemy && player->isAttacking) {
        SDL_Rect playerAttackBox = player->getAttackHitbox();
        SDL_Rect enemyCollider = enemy->getCollider();

        // Check if enemy's attack hits player
        if (SDL_HasIntersection(&playerAttackBox, &enemyCollider) &&
            !enemy->isTakingHit() && !enemy->isPermanentlyDisabled) {
            std::cout << "Enemy hit by player!" << std::endl;
            enemy->takeHit();
        }
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // 1. Render background layers from back to front
    if (backgroundTexture) {
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
    }

    // 2. Render middle layer (foreground) with parallax
    if (foregroundTexture) {
        int parallaxX = cameraX / 2;
        SDL_Rect srcRect = {
            parallaxX % SCREEN_WIDTH,
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, foregroundTexture, &srcRect, &destRect);

        if (srcRect.x > 0) {
            SDL_Rect wrapSrcRect = {0, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_Rect wrapDestRect = {SCREEN_WIDTH - srcRect.x, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, foregroundTexture, &wrapSrcRect, &wrapDestRect);
        }
    }

    // 3. Render closest layer (front layer) with faster parallax
    if (closestTexture) {
        int parallaxX = cameraX * 2;
        SDL_Rect srcRect = {
            parallaxX % SCREEN_WIDTH,
            0,
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        };
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, closestTexture, &srcRect, &destRect);

        if (srcRect.x > 0) {
            SDL_Rect wrapSrcRect = {0, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_Rect wrapDestRect = {SCREEN_WIDTH - srcRect.x, 0, srcRect.x, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, closestTexture, &wrapSrcRect, &wrapDestRect);
        }
    }

    // 4. Render tilemap after all background layers
    if (tileMap) {
        tileMap->render(cameraX, cameraY);
    }

    // 5. Render game objects
    if (player && player->getX() >= 0 && player->getY() >= 0) {
        int renderX = player->getX() - cameraX;
        int renderY = player->getY() - cameraY;
        // Only render the sprite here, hitboxes will be drawn later
        player->renderSprite(renderX, renderY);
    }

    if (enemy && enemy->getX() >= 0 && enemy->getY() >= 0) {
        int renderX = enemy->getX() - cameraX;
        int renderY = enemy->getY() - cameraY;
        enemy->render(renderX, renderY);
    }

    // 6. Render hitboxes on top of everything
    if (player && player->getX() >= 0 && player->getY() >= 0) {
        int renderX = player->getX() - cameraX;
        int renderY = player->getY() - cameraY;
        player->renderHitboxes(renderX, renderY);
    }

    // Finally present everything
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Cleanup audio before other resources
    AudioManager::getInstance().cleanup();

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
