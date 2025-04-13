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
#include <random>

using namespace std;

// Static member definitions
SDL_Renderer* Game::renderer = nullptr;

Game::Game() : window(nullptr), isRunning(false), player(nullptr),
               firstWaveDefeated(false),
               defeatedEnemyCount(0),
               secondMusicStarted(false),
               tileMap(nullptr),
               backgroundTexture(nullptr),
               foregroundTexture(nullptr),
               closestTexture(nullptr),
               cameraX(0), cameraY(0),
               lockCamera(true),
               font(nullptr),
               showParryText(false),
               parryTextStartTime(0),
               parryTextSize(48),
               parryTextColor{255, 255, 255, 255},
               successfulParryCount(0),
               timerStarted(false),  // Don't start timer immediately
               timerStartTime(0),
               hasStartedTimer(false)  // Initialize new flag
{ }

Game::~Game() {
}

void Game::init(const char* title, int xPos, int yPos, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        // Initialize SDL_ttf
        if (TTF_Init() < 0) {
            std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            isRunning = false;
            return;
        }

        window = SDL_CreateWindow(title, xPos, yPos, width, height, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, 0); // Initialize static renderer
        if (!renderer) { isRunning = false; return; }
        isRunning = true;
        IMG_Init(IMG_INIT_PNG);

        // Load font
        font = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 36);
        if (!font) {
            std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
            isRunning = false;
            return;
        }

        // Initialize audio manager
        if (!AudioManager::getInstance().init()) {
            std::cout << "Failed to initialize AudioManager!" << std::endl;
            isRunning = false;
            return;
        }

        // Play initial background music and set up next track
        AudioManager::getInstance().playMusic("audio/medieval-star-188280.mp3");
        AudioManager::getInstance().setMusicVolume(64);

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

        // Initialize first enemy
        spawnRandomEnemy();

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
    // Add AudioManager update at the start of the update loop
    AudioManager::getInstance().update();

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

    // Check if all enemies are defeated
    bool allEnemiesDefeated = true;
    for (Enemy* enemy : enemies) {
        if (enemy && !enemy->isPermanentlyDisabled) {
            allEnemiesDefeated = false;
            break;
        }
    }

    // Handle newly defeated enemies and spawn new ones if all are defeated
    if (allEnemiesDefeated) {
        // Spawn 2 enemies at different positions
        spawnRandomEnemy();
        spawnRandomEnemy();

        // Change music after defeating 10 enemies if not already changed
        if (defeatedEnemyCount >= 10 && !secondMusicStarted) {
            AudioManager::getInstance().playMusic("audio/medieval-adventure-270566.mp3");
            AudioManager::getInstance().setNextTrack("audio/medieval-opener-270568.mp3");
            AudioManager::getInstance().setMusicVolume(64);
            secondMusicStarted = true;
        }
    }

    // Update all enemies
    for (Enemy* enemy : enemies) {
        if (enemy && player) {
            enemy->prevX = enemy->getX();
            enemy->prevY = enemy->getY();

            // Check distance to player
            float dx = player->getX() - enemy->getX();
            float dy = player->getY() - enemy->getY();
            float distance = std::sqrt(dx * dx + dy * dy);

            // Make the enemy move toward the player or attack based on distance
            if (!enemy->isPermanentlyDisabled && !enemy->isInHitState) {
                if (distance > 55.0f) { // Reduced from 80.0f to 55.0f to get closer
                    enemy->aiMoveTowards(player->getX());
                } else {
                    enemy->velocityX = 0.0f;
                    enemy->tryAttack(player);
                    if (enemy->getState() != ENEMY_ATTACKING) {
                        enemy->setState(ENEMY_IDLE);
                    }
                }
            }

            Physics::applyGravity(enemy);
            Physics::applyFriction(enemy);
            enemy->update(player);

            // Handle enemy attacks hitting player
            if (enemy->getState() == ENEMY_ATTACKING) {
                SDL_Rect enemyAttackBox = enemy->getAttackHitbox();
                SDL_Rect playerCollider = player->getCollider();

                if (SDL_HasIntersection(&enemyAttackBox, &playerCollider)) {
                    if (player->isParrying) {
                        float parryKnockback = 10.0f;
                        player->velocityX = player->facingRight ? -parryKnockback : parryKnockback;
                        AudioManager::getInstance().playRandomParrySound();
                        
                        // Show parry text with random color when successful
                        showParryText = true;
                        parryTextStartTime = SDL_GetTicks();
                        successfulParryCount++;  // Increment successful parries
                        
                        // Generate random bright color
                        parryTextColor.r = rand() % 128 + 128; // 128-255 for brighter colors
                        parryTextColor.g = rand() % 128 + 128;
                        parryTextColor.b = rand() % 128 + 128;
                        parryTextColor.a = 255;
                    } else {
                        AudioManager::getInstance().playRandomHitSound();
                        player->takeHit();
                    }
                }
            }

            // Handle player attacks hitting enemy
            if (player->isAttacking) {
                SDL_Rect playerAttackBox = player->getAttackHitbox();
                SDL_Rect enemyCollider = enemy->getCollider();

                if (SDL_HasIntersection(&playerAttackBox, &enemyCollider) &&
                    !enemy->isTakingHit() && !enemy->isPermanentlyDisabled) {
                    AudioManager::getInstance().playRandomHitSound();
                    enemy->takeHit();
                    // Increment defeat count when enemy is hit and will be disabled
                    defeatedEnemyCount++;
                }
            }
        }
    }

    // Play miss sound for player attacks that don't hit anything
    if (player && player->isAttacking && player->getCurrentFrame() == 0) {
        bool hitSomething = false;
        SDL_Rect playerAttackBox = player->getAttackHitbox();

        for (Enemy* enemy : enemies) {
            if (enemy && !enemy->isPermanentlyDisabled) {
                SDL_Rect enemyCollider = enemy->getCollider();
                if (SDL_HasIntersection(&playerAttackBox, &enemyCollider)) {
                    hitSomething = true;
                    break;
                }
            }
        }

        if (!hitSomething) {
            AudioManager::getInstance().playMissSound();
        }
    }

    // Update parry text timer
    if (showParryText && SDL_GetTicks() - parryTextStartTime >= PARRY_TEXT_DURATION) {
        showParryText = false;
    }

    // Start timer when reaching 10 defeats if not already started
    if (!hasStartedTimer && defeatedEnemyCount >= 10) {
        hasStartedTimer = true;
        timerStarted = true;
        timerStartTime = SDL_GetTicks();
    }

    // End game when timer runs out (only if timer has started)
    if (timerStarted && SDL_GetTicks() - timerStartTime >= TIMER_DURATION) {
        isRunning = false;
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

    // Render all enemies
    for (Enemy* enemy : enemies) {
        if (enemy && enemy->getX() >= 0 && enemy->getY() >= 0) {
            int renderX = enemy->getX() - cameraX;
            int renderY = enemy->getY() - cameraY;
            enemy->render(renderX, renderY);
        }
    }

    // Render UI elements if font is available
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255}; // White text
        
        // Render enemy defeat counter
        std::string countText = "Enemies Defeated: " + std::to_string(defeatedEnemyCount);
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, countText.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            
            SDL_Rect textRect;
            textRect.w = textSurface->w;
            textRect.h = textSurface->h;
            textRect.x = (SCREEN_WIDTH - textRect.w) / 2;
            textRect.y = 20;
            
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            
            // Render parry counter
            std::string parryText = "PARRIED: " + std::to_string(successfulParryCount);
            SDL_Surface* parrySurface = TTF_RenderText_Solid(font, parryText.c_str(), textColor);
            if (parrySurface) {
                SDL_Texture* parryTexture = SDL_CreateTextureFromSurface(renderer, parrySurface);
                
                SDL_Rect parryCountRect;
                parryCountRect.w = parrySurface->w;
                parryCountRect.h = parrySurface->h;
                parryCountRect.x = (SCREEN_WIDTH - parryCountRect.w) / 2;
                parryCountRect.y = textRect.y + textRect.h + 10;
                
                SDL_RenderCopy(renderer, parryTexture, NULL, &parryCountRect);
                
                // Only render countdown timer if more than 10 enemies defeated
                if (timerStarted && defeatedEnemyCount >= 10) {
                    Uint32 elapsedTime = SDL_GetTicks() - timerStartTime;
                    Uint32 remainingTime = (elapsedTime >= TIMER_DURATION) ? 0 : TIMER_DURATION - elapsedTime;
                    
                    int minutes = (remainingTime / 1000) / 60;
                    int seconds = (remainingTime / 1000) % 60;
                    
                    std::string timerText = std::to_string(minutes) + " : " + 
                                          (seconds < 10 ? "0" : "") + std::to_string(seconds);
                    
                    SDL_Surface* timerSurface = TTF_RenderText_Solid(font, timerText.c_str(), textColor);
                    if (timerSurface) {
                        SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
                        
                        SDL_Rect timerRect;
                        timerRect.w = timerSurface->w;
                        timerRect.h = timerSurface->h;
                        timerRect.x = (SCREEN_WIDTH - timerRect.w) / 2;
                        timerRect.y = parryCountRect.y + parryCountRect.h + 10;
                        
                        SDL_RenderCopy(renderer, timerTexture, NULL, &timerRect);
                        
                        SDL_FreeSurface(timerSurface);
                        SDL_DestroyTexture(timerTexture);
                    }
                }
                
                SDL_FreeSurface(parrySurface);
                SDL_DestroyTexture(parryTexture);
            }
            
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        // Render parry effect text if active
        if (showParryText) {
            // Calculate size based on successful parries (start at 48, increase by 12 each time)
            int currentTextSize = parryTextSize + (successfulParryCount - 1) * 12;
            currentTextSize = std::min(currentTextSize, 120); // Cap maximum size at 120
            
            // Create a larger font for the parry text
            TTF_Font* largeFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", currentTextSize);
            if (largeFont) {
                SDL_Surface* parrySurface = TTF_RenderText_Solid(largeFont, "PARRY!?", parryTextColor);
                if (parrySurface) {
                    SDL_Texture* parryTexture = SDL_CreateTextureFromSurface(renderer, parrySurface);
                    
                    SDL_Rect parryRect;
                    parryRect.w = parrySurface->w;
                    parryRect.h = parrySurface->h;
                    parryRect.x = (SCREEN_WIDTH - parryRect.w) / 2;
                    parryRect.y = (SCREEN_HEIGHT / 2) - 50 - parryRect.h/2; // Center vertically with 50px offset
                    
                    SDL_RenderCopy(renderer, parryTexture, NULL, &parryRect);
                    
                    SDL_FreeSurface(parrySurface);
                    SDL_DestroyTexture(parryTexture);
                }
                TTF_CloseFont(largeFont);
            }
        }
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
    // Cleanup font before other resources
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();

    // Cleanup audio before other resources
    AudioManager::getInstance().cleanup();

    // Destroy textures - TextureManager::cleanUp() assumes it handles this
    TextureManager::cleanUp();
    delete player;
    player = nullptr;
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
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

void Game::spawnRandomEnemy() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100, SCREEN_WIDTH - 100); // Use screen width for spawning

    // Random x position anywhere on screen
    int spawnX = dist(gen);

    Enemy* newEnemy = new Enemy("assets/Idle.png", "assets/Run.png", "assets/Attack.png",
                               "assets/Take Hit.png", "assets/Death.png", spawnX, 0, 1.0f);
    if (newEnemy) {
        enemies.push_back(newEnemy);
    }
}
