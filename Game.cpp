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
int Game::SCREEN_WIDTH = 720;  // Initial default values
int Game::SCREEN_HEIGHT = 576;

// Define mastery animation frame coordinates
const Game::MasteryFrame Game::MASTERY_FRAMES[30] = {
    // Row 1
    {30, 10, 60, 55}, {160, 10, 60, 55}, {290, 10, 60, 55}, {420, 10, 60, 55}, {550, 10, 60, 55},
    {680, 10, 60, 55}, {810, 10, 60, 55}, {940, 10, 60, 55}, {1070, 10, 60, 55}, {1200, 10, 60, 55},
    // Row 2 (wider icons)
    {30, 80, 60, 50}, {160, 80, 60, 50}, {290, 80, 60, 50}, {420, 80, 60, 50}, {550, 80, 60, 50},
    {670, 75, 80, 60}, {800, 75, 80, 60}, {930, 75, 80, 60}, {1060, 75, 80, 60}, {1190, 75, 80, 60},
    // Row 3 (consistently wider)
    {20, 145, 80, 60}, {150, 145, 80, 60}, {280, 145, 80, 60}, {410, 145, 80, 60}, {540, 145, 80, 60},
    {670, 145, 80, 60}, {800, 145, 80, 60}, {930, 145, 80, 60}, {1060, 145, 80, 60}, {1190, 145, 80, 60}
};

Game::Game() : 
    window(nullptr),
    isRunning(false),
    player(nullptr),
    enemies(),
    firstWaveDefeated(false),
    defeatedEnemyCount(0),
    tileMap(nullptr),
    backgroundTexture(nullptr),
    foregroundTexture(nullptr),
    closestTexture(nullptr),
    cameraX(0),
    cameraY(0),
    lockCamera(true),
    secondMusicStarted(false),
    font(nullptr),
    showParryText(false),
    parryTextStartTime(0),
    parryTextColor{255, 255, 255, 255},
    parryTextSize(48),
    successfulParryCount(0),
    timerStarted(false),
    timerStartTime(0),
    hasStartedTimer(false),
    masteryTexture(nullptr),
    showMasteryAnimation(false),
    masteryStartTime(0),
    masteryFrame(0),
    isFading(false),
    fadeStartTime(0),
    fadeAlpha(0),
    showDeathText(false),
    deathTextStartTime(0),
    deathTextAlpha(0),
    canRestart(false),
    isPaused(false),
    pauseScreenTexture(nullptr),
    showInstructions(false),
    instructionsTexture(nullptr),
    showInitialInstructions(true)
{ }

Game::~Game() {
}

void Game::init(const char* title, int xPos, int yPos, int width, int height) {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        // Create window normally (not fullscreen)
        window = SDL_CreateWindow(title, xPos, yPos, width, height, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cout << "Failed to create window! SDL Error: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cout << "Failed to create renderer! SDL Error: " << SDL_GetError() << std::endl;
            isRunning = false;
            return;
        }

        isRunning = true;
        IMG_Init(IMG_INIT_PNG);

        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            isRunning = false;
            return;
        }

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
        masteryTexture = TextureManager::loadTexture("assets/mastery 7.png");

        // Load pause screen texture
        pauseScreenTexture = TextureManager::loadTexture("assets/999707.png");
        if (!pauseScreenTexture) {
            std::cout << "Failed to load pause screen texture!" << std::endl;
        }
        
        // Load instructions texture
        instructionsTexture = TextureManager::loadTexture("assets/instruction.png");
        if (!instructionsTexture) {
            std::cout << "Failed to load instructions texture!" << std::endl;
        }
        
        showInstructions = false;  // Regular instructions flag (for F1)
        showInitialInstructions = true;  // Show instructions at startup
        isPaused = true;  // Pause the game initially
        AudioManager::getInstance().pauseMusic();  // Pause music initially

        // Initialize player with size 50x50
        player = new GameObject(50, 50, 50, 50);
        if (player == nullptr) {
            std::cerr << "Failed to create player object!" << std::endl;
            isRunning = false;
            return;
        }
        player->setGameRef(this);  // Set reference to game instance

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
                case SDLK_RETURN:  // Enter key
                    if (showInitialInstructions) {
                        showInitialInstructions = false;
                        isPaused = false;
                        AudioManager::getInstance().resumeMusic();
                    }
                    break;
                case SDLK_ESCAPE:
                    isPaused = !isPaused;
                    showInstructions = false;  // Always hide instructions when ESC is pressed
                    if (isPaused) {
                    // Pause both music and sound effects
                        AudioManager::getInstance().pauseMusic();
                        AudioManager::getInstance().pauseAllSounds();
                    } else {
                        // Resume both music and sound effects
                        AudioManager::getInstance().resumeMusic();
                        AudioManager::getInstance().resumeAllSounds();
                    }
                    break;
                case SDLK_F1:
                    if (!isPaused) {
                        isPaused = true;
                        showInstructions = true;
                        // Pause both music and sound effects
                        AudioManager::getInstance().pauseMusic();
                        AudioManager::getInstance().pauseAllSounds();
                    } else if (showInstructions) {
                        isPaused = false;
                        showInstructions = false;
                        // Resume both music and sound effects
                        AudioManager::getInstance().resumeMusic();
                        AudioManager::getInstance().resumeAllSounds();
                    }
                    break;
                case SDLK_w:
                    if (!isPaused) player->jump();
                    break;
                case SDLK_SPACE:
                    if (!isPaused) player->dash();
                    break;
                case SDLK_F5:
                    // Only allow restart if player is dead and reset text is fully visible
                    if (player && player->permanentlyDisabled && showDeathText && deathTextAlpha >= 255) {
                        restart();
                    }
                    break;
                case SDLK_s:
                    break;
                case SDLK_a:
                case SDLK_d:
                    break;
                default:
                    break;
            }
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            // Handle initial instructions dismissal
            if (showInitialInstructions) {
                showInitialInstructions = false;
                isPaused = false;
                AudioManager::getInstance().resumeMusic();
                return;
            }

            // Handle end game screen click to resume
            if (showEndGameScreen) {
                    showEndGameScreen = false;
                    isPaused = false;
                    // Reset timer so it doesn't immediately trigger again
                    timerStarted = false;
                    hasStartedTimer = false;
                    // Resume music
                    AudioManager::getInstance().resumeMusic();
                    return; // Important to prevent processing other mouse actions
            }

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
        if (event.type == SDL_MOUSEBUTTONDOWN && showEndGameScreen) {
            showEndGameScreen = false;
            isPaused = false;
            AudioManager::getInstance().resumeMusic();
        }
    }
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_A]) player->move(-1);
    if (keyState[SDL_SCANCODE_D]) player->move(1);
}

void Game::update() {
    // Always update audio manager even when paused
    AudioManager::getInstance().update();

    // Don't update game logic if paused
    if (isPaused) {
        return;
    }

    // Check if the player is dead and start death effects
    if (player && player->permanentlyDisabled) {
        // Stop background music when player dies (will stop every time)
        AudioManager::getInstance().stopMusic();

        // If death animation is complete, start fade
        if (player->isDeathAnimationComplete()) {
            isFading = true;
            if (fadeStartTime == 0) { // Only set start time once
                fadeStartTime = SDL_GetTicks();
            }
        }

        // Start death text only after fade is complete
        if (isFading && fadeAlpha >= 255 && !showDeathText) {
            showDeathText = true;
            deathTextStartTime = SDL_GetTicks();
            deathTextAlpha = 0;
            // Play death sound effect
            AudioManager::getInstance().playSoundEffect("audio/Dark Souls - You Died (Sound Effect).mp3");
        }
    }

    // Update fade effect
    if (isFading) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - fadeStartTime;

        if (elapsedTime >= FADE_DURATION) {
            fadeAlpha = 255;  // Keep screen black
        } else {
            // Gradually increase alpha from 0 to 255 over FADE_DURATION
            fadeAlpha = static_cast<Uint8>((elapsedTime * 255) / FADE_DURATION);
        }
    }

    // Update death text fade in
    if (showDeathText) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deathTextElapsed = currentTime - deathTextStartTime;

        if (deathTextElapsed >= DEATH_TEXT_DURATION) {
            deathTextAlpha = 255;  // Keep text fully visible
        } else {
            // Gradually increase alpha from 0 to 255
            deathTextAlpha = static_cast<Uint8>((deathTextElapsed * 255) / DEATH_TEXT_DURATION);
        }
    }

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
                    if (player->isDashing) {
                        if (!player->hasParriedDuringDash) {
                            // Successful parry during dash
                            AudioManager::getInstance().playRandomParrySound();
                            player->hasParriedDuringDash = true;

                            // Show parry text
                            showParryText = true;
                            parryTextStartTime = SDL_GetTicks();
                            successfulParryCount++;

                            // Generate random bright color
                            parryTextColor.r = rand() % 128 + 128;
                            parryTextColor.g = rand() % 128 + 128;
                            parryTextColor.b = rand() % 128 + 128;
                            parryTextColor.a = 255;
                        }
                        // During dash, always ignore hit regardless of parry status
                        return;
                    } else if (player->isParrying) {
                        // Normal parry when not dashing
                        float parryKnockback = 10.0f;
                        player->velocityX = player->facingRight ? -parryKnockback : parryKnockback;
                        AudioManager::getInstance().playRandomParrySound();

                        showParryText = true;
                        parryTextStartTime = SDL_GetTicks();
                        successfulParryCount++;

                        parryTextColor.r = rand() % 128 + 128;
                        parryTextColor.g = rand() % 128 + 128;
                        parryTextColor.b = rand() % 128 + 128;
                        parryTextColor.a = 255;
                    } else {
                        // Only take hit if not dashing and not parrying
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
                    // Increment both game and player defeat counts
                    defeatedEnemyCount++;
                    player->incrementEnemiesDefeated();
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

    // Update mastery animation if player defeats 5 enemies and is alive
    if (defeatedEnemyCount == 5 && !showMasteryAnimation && player && !player->permanentlyDisabled) {
        showMasteryAnimation = true;
        masteryStartTime = SDL_GetTicks();
        masteryFrame = 0;
        // Play both mastery sound effects
        AudioManager::getInstance().playSoundEffect("audio/mastery_emote_tier5.mp3");
        AudioManager::getInstance().playSoundEffect("audio/death-is-like-the-wind-always-by-my-side-101soundboards.mp3");
    }

    // Update mastery animation frames
    if (showMasteryAnimation) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - masteryStartTime;
        int expectedFrame = (elapsedTime / MASTERY_FRAME_DURATION) % MASTERY_TOTAL_FRAMES;

        if (expectedFrame != masteryFrame) {
            masteryFrame = expectedFrame;
            if (masteryFrame >= MASTERY_TOTAL_FRAMES - 1) {
                showMasteryAnimation = false;
            }
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
    // Update timer only if it's started and player is alive
    if (timerStarted && !player->permanentlyDisabled && SDL_GetTicks() - timerStartTime >= TIMER_DURATION) {
        isPaused = true;
        showEndGameScreen = true;
        AudioManager::getInstance().pauseMusic();
    }

    // Update death count when player dies
    if (player && player->permanentlyDisabled && !player->deathCountUpdated) {
        // Read current death count
        std::ifstream deathFile("deathCount.txt");
        int deathCount = 0;
        if (deathFile.is_open()) {
            deathFile >> deathCount;
            deathFile.close();
        }

        // Increment and write back
        deathCount++;
        std::ofstream outFile("deathCount.txt");
        if (outFile.is_open()) {
            outFile << deathCount;
            outFile.close();
        }

        player->deathCountUpdated = true;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Draw all game elements first
    // 1. Render background layers from back to front

    // Handle end game screen
    if (showEndGameScreen) {
        // Fill screen with black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render "thank for the suffering <3" text and death count
        if (font) {
            SDL_Color textColor = {255, 255, 255, 255}; // White color

            // Render main text
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, "thank for the suffering ⸜(｡˃ ᵕ ˂ )⸝♡", textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                if (textTexture) {
                    // Center the text
                    SDL_Rect textRect;
                    textRect.w = textSurface->w;
                    textRect.h = textSurface->h;
                    textRect.x = (SCREEN_WIDTH - textRect.w) / 2;
                    textRect.y = (SCREEN_HEIGHT - textRect.h) / 2;

                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }

            // Read and render death count
            std::ifstream deathFile("deathCount.txt");
            int deathCount = 0;
            if (deathFile.is_open()) {
                deathFile >> deathCount;
                deathFile.close();
            }

            std::string deathText = "you died " + std::to_string(deathCount) + " times";
            SDL_Surface* deathSurface = TTF_RenderText_Blended(font, deathText.c_str(), textColor);
            if (deathSurface) {
                SDL_Texture* deathTexture = SDL_CreateTextureFromSurface(renderer, deathSurface);
                if (deathTexture) {
                    SDL_Rect deathRect;
                    deathRect.w = deathSurface->w;
                    deathRect.h = deathSurface->h;
                    deathRect.x = (SCREEN_WIDTH - deathRect.w) / 2;
                    deathRect.y = (SCREEN_HEIGHT - deathRect.h) / 2 + 40; // Position below main text

                    SDL_RenderCopy(renderer, deathTexture, NULL, &deathRect);
                    SDL_DestroyTexture(deathTexture);
                }
                SDL_FreeSurface(deathSurface);
            }

            // Add "click to continue" text
            SDL_Surface* clickSurface = TTF_RenderText_Blended(font, "click to continue", textColor);
            if (clickSurface) {
                SDL_Texture* clickTexture = SDL_CreateTextureFromSurface(renderer, clickSurface);
                if (clickTexture) {
                    SDL_Rect clickRect;
                    clickRect.w = clickSurface->w;
                    clickRect.h = clickSurface->h;
                    clickRect.x = (SCREEN_WIDTH - clickRect.w) / 2;
                    clickRect.y = (SCREEN_HEIGHT - clickRect.h) / 2 + 80; // Position below death count

                    SDL_RenderCopy(renderer, clickTexture, NULL, &clickRect);
                    SDL_DestroyTexture(clickTexture);
                }
                SDL_FreeSurface(clickSurface);
            }
        }

        SDL_RenderPresent(renderer);
        return;
    }

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

    // Render cooldown indicators in top right corner
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255}; // White color
        TTF_Font* smallFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 24);
        if (smallFont && player) {
            int yOffset = 20;
            Uint32 currentTime = SDL_GetTicks();

            // Attack cooldown
            std::string attackIndicator = "ATTACK : ";
            float attackCooldownPercent = std::min(1.0f, float(currentTime - player->lastAttackTime) / GameObject::ATTACK_COOLDOWN);
            attackIndicator += std::string(5 - int(attackCooldownPercent * 5), '*') + std::string(int(attackCooldownPercent * 5), '=');

            // Parry cooldown
            std::string parryIndicator = "PARRY  : ";
            float parryCooldownPercent = std::min(1.0f, float(currentTime - player->lastParryTime) / GameObject::PARRY_COOLDOWN);
            parryIndicator += std::string(5 - int(parryCooldownPercent * 5), '*') + std::string(int(parryCooldownPercent * 5), '=');

            // Dash cooldown (only show after 15 enemy defeats)
            std::string dashIndicator = "DASH   : ";
            if (player->getEnemiesDefeated() >= GameObject::MIN_ENEMIES_FOR_DASH) {
                // Calculate current dash cooldown
                Uint32 currentCooldown = GameObject::BASE_DASH_COOLDOWN;
                int excessEnemies = player->getEnemiesDefeated() - GameObject::MIN_ENEMIES_FOR_DASH;
                if (excessEnemies > 0) {
                    Uint32 reduction = excessEnemies * GameObject::COOLDOWN_REDUCTION_PER_ENEMY;
                    if (reduction > GameObject::BASE_DASH_COOLDOWN) {
                        currentCooldown = 0;
                    } else {
                        currentCooldown -= reduction;
                    }
                }
                float dashCooldownPercent = std::min(1.0f, float(currentTime - player->lastDashTime) / currentCooldown);
                dashIndicator += std::string(5 - int(dashCooldownPercent * 5), '*') + std::string(int(dashCooldownPercent * 5), '=');
            } else {
                dashIndicator += "LOCKED";
            }

            // Render each indicator
            const char* indicators[] = {attackIndicator.c_str(), parryIndicator.c_str(), dashIndicator.c_str()};
            for (const char* text : indicators) {
                SDL_Surface* surface = TTF_RenderText_Solid(smallFont, text, textColor);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect rect;
                    rect.w = surface->w;
                    rect.h = surface->h;
                    rect.x = SCREEN_WIDTH - rect.w - 20;
                    rect.y = yOffset;
                    SDL_RenderCopy(renderer, texture, NULL, &rect);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                    yOffset += rect.h + 5;
                }
            }
            TTF_CloseFont(smallFont);
        }
    }

    // 6. Render hitboxes on top of everything
    if (player && player->getX() >= 0 && player->getY() >= 0) {
        int renderX = player->getX() - cameraX;
        int renderY = player->getY() - cameraY;
        player->renderHitboxes(renderX, renderY);
    }

    // Render fade overlay on top of everything if active
    if (isFading) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, fadeAlpha);
        SDL_Rect fadeRect = {-cameraX, -cameraY, MAP_COLS * TILE_SIZE, MAP_ROWS * TILE_SIZE};
        SDL_RenderFillRect(renderer, &fadeRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // Draw end game screen overlay and text if active - rendered last to appear on top of everything
    if (showEndGameScreen && font) {
        // Create semi-transparent black overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect fullscreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullscreen);

        // Render "thanks for the suffering <3" text
        TTF_SetFontSize(font, 48);
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "thanks for the suffering <3", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    (SCREEN_WIDTH - textSurface->w) / 2,
                    (SCREEN_HEIGHT - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // Render mastery animation if active (and not during death or pause)
    if (showMasteryAnimation && masteryTexture && !showDeathText && !isPaused) {
        const MasteryFrame& frame = MASTERY_FRAMES[masteryFrame];

        SDL_Rect srcRect = {
            frame.x,
            frame.y,
            frame.w,
            frame.h
        };

        // Center the mastery animation above the player with 1.5x scaling
        SDL_Rect destRect = {
            player->getX() - cameraX + (player->destRect.w - (int)(frame.w * 1.5f)) / 2,
            player->getY() - cameraY - (int)(frame.h * 1.5f) - 20,  // Position above player with 20px gap
            (int)(frame.w * 1.5f),
            (int)(frame.h * 1.5f)
        };

        SDL_RenderCopy(renderer, masteryTexture, &srcRect, &destRect);
    }

    // Render the "YOU DIED" text and reset prompt after fade effect
    if (showDeathText && font) {
        // Render YOU DIED text
        TTF_Font* deathFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 72);
        if (deathFont) {
            SDL_Color deathTextColor = {255, 0, 0, deathTextAlpha};  // Red color with fading alpha
            SDL_Surface* deathSurface = TTF_RenderText_Solid(deathFont, "YOU DIED", deathTextColor);
            if (deathSurface) {
                SDL_Texture* deathTexture = SDL_CreateTextureFromSurface(renderer, deathSurface);
                SDL_SetTextureAlphaMod(deathTexture, deathTextAlpha);

                SDL_Rect deathRect;
                deathRect.w = deathSurface->w;
                deathRect.h = deathSurface->h;
                deathRect.x = (SCREEN_WIDTH - deathRect.w) / 2;
                deathRect.y = (SCREEN_HEIGHT - deathRect.h) / 2;

                SDL_RenderCopy(renderer, deathTexture, NULL, &deathRect);

                // Only show reset prompt when YOU DIED is fully visible
                if (deathTextAlpha >= 255) {
                    SDL_Color resetTextColor = {255, 255, 255, 255}; // White color for reset text
                    SDL_Surface* resetSurface = TTF_RenderText_Solid(deathFont, "Press F5 to reset", resetTextColor);
                    if (resetSurface) {
                        SDL_Texture* resetTexture = SDL_CreateTextureFromSurface(renderer, resetSurface);

                        SDL_Rect resetRect;
                        resetRect.w = resetSurface->w;
                        resetRect.h = resetSurface->h;
                        resetRect.x = (SCREEN_WIDTH - resetRect.w) / 2;
                        resetRect.y = deathRect.y + deathRect.h + 20; // 20 pixels below YOU DIED text

                        SDL_RenderCopy(renderer, resetTexture, NULL, &resetRect);

                        SDL_FreeSurface(resetSurface);
                        SDL_DestroyTexture(resetTexture);
                    }
                }

                SDL_FreeSurface(deathSurface);
                SDL_DestroyTexture(deathTexture);
            }
            TTF_CloseFont(deathFont);
        }
    }

    // Render pause screen overlay if game is paused
    if (isPaused) {
        // First draw a semi-transparent black overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192); // 75% opacity black
        SDL_Rect fullscreenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullscreenRect);

        if (pauseScreenTexture) {
            // Calculate dimensions to maintain aspect ratio while filling screen
            int imgW, imgH;
            SDL_QueryTexture(pauseScreenTexture, NULL, NULL, &imgW, &imgH);
            float imgAspect = (float)imgW / imgH;
            float screenAspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

            SDL_Rect pauseRect;
            if (screenAspect > imgAspect) {
                // Screen is wider than image
                pauseRect.w = SCREEN_WIDTH;
                pauseRect.h = (int)(SCREEN_WIDTH / imgAspect);
                pauseRect.x = 0;
                pauseRect.y = (SCREEN_HEIGHT - pauseRect.h) / 2;
            } else {
                // Screen is taller than image
                pauseRect.h = SCREEN_HEIGHT;
                pauseRect.w = (int)(SCREEN_HEIGHT * imgAspect);
                pauseRect.x = (SCREEN_WIDTH - pauseRect.w) / 2;
                pauseRect.y = 0;
            }

            SDL_RenderCopy(renderer, pauseScreenTexture, NULL, &pauseRect);
        }

        // Render "PAUSED" text and subtitle
        if (font) {
            TTF_Font* pauseFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 72);
            if (pauseFont) {
                SDL_Color pauseTextColor = {255, 0, 0, 255}; // Red color
                SDL_Surface* pauseSurface = TTF_RenderText_Solid(pauseFont, "PAUSED", pauseTextColor);
                if (pauseSurface) {
                    SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);

                    SDL_Rect textRect;
                    textRect.w = pauseSurface->w;
                    textRect.h = pauseSurface->h;
                    textRect.x = (SCREEN_WIDTH - textRect.w) / 2;
                    textRect.y = (SCREEN_HEIGHT - textRect.h) / 2;

                    SDL_RenderCopy(renderer, pauseTexture, NULL, &textRect);

                    // Add "Press ESC" subtitle in blue
                    TTF_Font* smallFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 36);
                    if (smallFont) {
                        SDL_Color escTextColor = {0, 0, 255, 255}; // Blue color
                        SDL_Surface* escSurface = TTF_RenderText_Solid(smallFont, "Press ESC", escTextColor);
                        if (escSurface) {
                            SDL_Texture* escTexture = SDL_CreateTextureFromSurface(renderer, escSurface);

                            SDL_Rect escRect;
                            escRect.w = escSurface->w;
                            escRect.h = escSurface->h;
                            escRect.x = (SCREEN_WIDTH - escRect.w) / 2;
                            escRect.y = textRect.y + textRect.h + 20; // 20 pixels below PAUSED text

                            SDL_RenderCopy(renderer, escTexture, NULL, &escRect);

                            SDL_FreeSurface(escSurface);
                            SDL_DestroyTexture(escTexture);
                        }
                        TTF_CloseFont(smallFont);
                    }

                    SDL_FreeSurface(pauseSurface);
                    SDL_DestroyTexture(pauseTexture);
                }
                TTF_CloseFont(pauseFont);
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // Present the rendered frame
    // 7. Render instruction screen if active
    if (showInstructions && instructionsTexture) {
        SDL_Rect destRect = {
            (SCREEN_WIDTH - 600) / 2,  // Center horizontally
            (SCREEN_HEIGHT - 400) / 2, // Center vertically
            600,
            400
        };
        SDL_RenderCopy(renderer, instructionsTexture, NULL, NULL);
    }
    if (font && !showDeathText) {
    SDL_Color textColor = {255, 255, 255, 255}; // White color (R,G,B,A)
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "F1 for instruction", textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;
        textRect.x = 0;
        textRect.y = 50;  // 50 pixels from top

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}
    // 8. Present renderer
    // Show initial instructions
    if (showInitialInstructions && instructionsTexture) {
        // Semi-transparent black overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
        SDL_Rect fullscreenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &fullscreenRect);

        // Render the instruction texture
        SDL_RenderCopy(renderer, instructionsTexture, NULL, NULL);

        // Add "Press Enter to start" text
        if (font) {
            TTF_Font* startFont = TTF_OpenFont("font/Jacquard_12/Jacquard12-Regular.ttf", 36);
            if (startFont) {
                SDL_Color startTextColor = {255, 255, 255, 255};
                SDL_Surface* startSurface = TTF_RenderText_Solid(startFont, "Press Enter to start", startTextColor);
                if (startSurface) {
                    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);

                    SDL_Rect startRect;
                    startRect.w = startSurface->w;
                    startRect.h = startSurface->h;
                    startRect.x = (SCREEN_WIDTH - startRect.w) / 2;
                    startRect.y = SCREEN_HEIGHT - 100;  // 100 pixels from bottom

                    SDL_RenderCopy(renderer, startTexture, NULL, &startRect);

                    SDL_FreeSurface(startSurface);
                    SDL_DestroyTexture(startTexture);
                }
                TTF_CloseFont(startFont);
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Cleanup font before other resources
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();  // Make sure TTF is properly quit

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

void Game::restart() {
    // Play restart sound
    AudioManager::getInstance().playSoundEffect("audio/ahhyooaaawhoaaa.mp3");

    // Reset game state variables
    defeatedEnemyCount = 0;
    successfulParryCount = 0;
    firstWaveDefeated = false;
    secondMusicStarted = false;
    timerStarted = false;
    hasStartedTimer = false;
    isFading = false;
    fadeAlpha = 0;
    showDeathText = false;
    deathTextAlpha = 0;

    // Clear existing enemies
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    // Recreate player
    delete player;
    player = new GameObject(50, 50, 50, 50);
    if (player) {
        player->setGameRef(this);
    }

    // Reset camera position
    cameraX = 0;
    cameraY = 0;

    // Spawn initial enemy
    spawnRandomEnemy();

    // Reset music to initial track
    AudioManager::getInstance().playMusic("audio/medieval-star-188280.mp3");
    AudioManager::getInstance().setMusicVolume(64);
}
