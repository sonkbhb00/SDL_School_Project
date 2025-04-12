#include "Enemy.h"
#include "TextureManager.h"
#include "Game.hpp"
#include "Physics.hpp" // Make sure Physics is included
#include <SDL.h>
#include <iostream>
#include <cstring> // Required for strcmp
#include <cmath>   // For std::fabs

// Original Helper function
bool Enemy::loadEnemyAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight) {
    texture = TextureManager::loadTexture(path); // Uses Game::renderer via TM
    if (!texture) return false;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameHeight = h;
    // Frame counts logic...
    if (strcmp(path, "assets/Idle.png") == 0) totalFrames = 11;
    else if (strcmp(path, "assets/Run.png") == 0) totalFrames = 8;
    else if (strcmp(path, "assets/Take Hit.png") == 0) totalFrames = 4;
    else if (strcmp(path, "assets/Death.png") == 0) totalFrames = 9;
    else totalFrames = 1;
    if (totalFrames > 0) frameWidth = w / totalFrames;
    else { frameWidth = w; totalFrames = 1; }
    return true;
}

// Original Constructor
Enemy::Enemy(const char* idleTexturePath, const char* runTexturePath, const char* takeHitTexturePath, const char* deathTexturePath,
             int x, int y, float scale, int idleAnimSpeed, int runAnimSpeed, int takeHitAnimSpeed, int deathAnimSpeed)
    : prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f), 
      onGround(false),
      isInHitState(false),
      isPermanentlyDisabled(false),
      currentState(ENEMY_IDLE),
      currentFrame(0),
      deathTotalFrames(0),
      xpos(x), ypos(y), 
      scale(scale),
      facingRight(true),
      idleTexture(nullptr), runTexture(nullptr), takeHitTexture(nullptr), deathTexture(nullptr), currentTexture(nullptr),
      idleTotalFrames(0), runTotalFrames(0), takeHitTotalFrames(0), currentTotalFrames(0),
      idleFrameWidth(0), idleFrameHeight(0), runFrameWidth(0), runFrameHeight(0), 
      takeHitFrameWidth(0), takeHitFrameHeight(0), deathFrameWidth(0), deathFrameHeight(0), currentFrameWidth(0), currentFrameHeight(0),
      idleAnimSpeed(idleAnimSpeed), runAnimSpeed(runAnimSpeed), takeHitAnimSpeed(takeHitAnimSpeed), deathAnimSpeed(deathAnimSpeed), currentAnimSpeed(idleAnimSpeed),
      lastFrameTime(SDL_GetTicks()),
      takeHitStartTime(0),
      takeHitDuration(0)
{
    // Load textures using original helper
    bool idleLoaded = loadEnemyAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight);
    bool runLoaded = loadEnemyAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    bool takeHitLoaded = loadEnemyAnimationData(takeHitTexturePath, takeHitTexture, takeHitTotalFrames, takeHitFrameWidth, takeHitFrameHeight);
    bool deathLoaded = loadEnemyAnimationData(deathTexturePath, deathTexture, deathTotalFrames, deathFrameWidth, deathFrameHeight);
    
    // Add checks for critical animations
    if (!idleLoaded && !runLoaded) { 
        std::cerr << "Error: Failed to load critical Idle/Run animations for Enemy!" << std::endl;
        // Handle error appropriately, maybe set a default state or return
    }
    if (!takeHitLoaded) {
        std::cerr << "Warning: Failed to load Take Hit animation for Enemy!" << std::endl;
        // takeHit functionality might be broken
    }
    if (!deathLoaded) {
        std::cerr << "Warning: Failed to load Death animation for Enemy!" << std::endl;
        // death functionality might be broken
    }

    if (idleLoaded) setAnimation(ENEMY_IDLE);
    else if (runLoaded) setAnimation(ENEMY_RUNNING);
    // ... fallbacks ...
    if (currentTexture) {
       destRect = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };
       srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };
    } else {
       destRect = {x, y, 32, 32}; srcRect = {0, 0, 0, 0};
    }
    std::cout << "Enemy created." << std::endl;
}

Enemy::~Enemy() { /* Textures handled by TM */ }

// Original update signature
void Enemy::update(const int mapData[][Game::MAP_COLS]) {
    // Original animation update logic
    Uint32 currentTime = SDL_GetTicks();
    if (isInHitState) {
        if (currentTime - takeHitStartTime >= takeHitDuration) {
            isInHitState = false;
            isPermanentlyDisabled = true;
            setAnimation(ENEMY_DEATH);
            velocityX = 0.0f; 
        }
    }
    if (!isInHitState && currentState != ENEMY_DEATH) { 
        if (isPermanentlyDisabled) {
             if (currentState != ENEMY_IDLE) setAnimation(ENEMY_IDLE);
        } else if (!onGround) {
             if (currentState != ENEMY_IDLE) setAnimation(ENEMY_IDLE); 
        } else if (std::fabs(velocityX) > 0.1f) {
            if (currentState != ENEMY_RUNNING) setAnimation(ENEMY_RUNNING);
        } else {
            if (currentState != ENEMY_IDLE) setAnimation(ENEMY_IDLE);
        }
    } 
    if (currentTexture && currentTotalFrames > 1 && currentTime - lastFrameTime >= static_cast<Uint32>(currentAnimSpeed)) {
        bool advanceFrame = true;
        if (currentState == ENEMY_DEATH || currentState == ENEMY_TAKE_HIT) { 
            if (currentFrame >= currentTotalFrames - 1) advanceFrame = false;
        }
        if(advanceFrame) {
            currentFrame = (currentFrame + 1);
            if (currentState != ENEMY_DEATH && currentState != ENEMY_TAKE_HIT) currentFrame %= currentTotalFrames;
            else currentFrame = std::min(currentFrame, currentTotalFrames - 1);
        }
        lastFrameTime = currentTime;
    }
    if(currentTexture) {
        srcRect.x = currentFrame * currentFrameWidth;
        srcRect.y = 0;
        srcRect.w = currentFrameWidth;
        srcRect.h = currentFrameHeight;
        destRect.w = static_cast<int>(currentFrameWidth * scale);
        destRect.h = static_cast<int>(currentFrameHeight * scale);
    }
}

void Enemy::setAnimation(EnemyAnimationState newState) {
    // Don't change animation state if already in that state
    if (currentState == newState) return;
    
    // Change current state and reset frame
    currentState = newState;
    currentFrame = 0;
    lastFrameTime = SDL_GetTicks();
    
    std::cout << "Enemy animation state changed to: ";
    switch(newState) {
        case ENEMY_IDLE: std::cout << "IDLE"; break;
        case ENEMY_RUNNING: std::cout << "RUNNING"; break;
        case ENEMY_JUMPING: std::cout << "JUMPING"; break;
        case ENEMY_TAKE_HIT: std::cout << "TAKE_HIT"; break;
        case ENEMY_DEATH: std::cout << "DEATH"; break;
        default: std::cout << "UNKNOWN"; break;
    }
    std::cout << std::endl;
    
    // Set appropriate texture and animation data based on new state
    switch(newState) {
        case ENEMY_IDLE:
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            break;
        case ENEMY_RUNNING:
            currentTexture = runTexture;
            currentTotalFrames = runTotalFrames;
            currentFrameWidth = runFrameWidth;
            currentFrameHeight = runFrameHeight;
            currentAnimSpeed = runAnimSpeed;
            break;
        case ENEMY_TAKE_HIT:
            currentTexture = takeHitTexture;
            currentTotalFrames = takeHitTotalFrames;
            currentFrameWidth = takeHitFrameWidth;
            currentFrameHeight = takeHitFrameHeight;
            currentAnimSpeed = takeHitAnimSpeed;
            break;
        case ENEMY_DEATH:
            currentTexture = deathTexture;
            currentTotalFrames = deathTotalFrames;
            currentFrameWidth = deathFrameWidth;
            currentFrameHeight = deathFrameHeight;
            currentAnimSpeed = deathAnimSpeed;
            std::cout << "Death animation set with " << deathTotalFrames << " frames at " << deathAnimSpeed << " speed" << std::endl;
            break;
        default:
            // Default to idle if unknown state
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            break;
    }
    
    // Reset collider width & height based on new animation
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
}

void Enemy::takeHit() {
    // If not already in the temporary hit state, trigger it
    if (!isInHitState) {
        isInHitState = true;
        takeHitStartTime = SDL_GetTicks();

        // Calculate duration based on loaded frame count and speed
        takeHitDuration = takeHitTotalFrames * takeHitAnimSpeed;
        // Ensure a minimum duration just in case
        if (takeHitDuration <= 0) takeHitDuration = 200; // Minimum 200ms stun if calculation fails

        // Set the temporary hit animation
        setAnimation(ENEMY_TAKE_HIT);

        // Stop horizontal movement and apply vertical pop
        velocityX = 0.0f;
        velocityY = -2.0f;
        onGround = false;

        std::cout << "Enemy taking hit! Frames: " << takeHitTotalFrames << ", Duration: " << takeHitDuration << "ms" << std::endl;
        
        // We'll set isPermanentlyDisabled at the end of the hit animation
        // This allows the TakeHit animation to complete before Death plays
    }
    
    // Ensure velocity stays zeroed if somehow hit again
    velocityX = 0.0f;
}

// AI movement - sets velocity based on target
void Enemy::aiMoveTowards(int targetX) {
    // Get current position
    int currentX = xpos;
    
    // Determine direction to move
    if (targetX < currentX) {
        // Target is to the left, so move left
        velocityX = -2.0f; // Slightly slower than player's speed
        facingRight = false; // Face left
        // Let update() handle animation state
    } else if (targetX > currentX) {
        // Target is to the right, so move right
        velocityX = 2.0f; // Slightly slower than player's speed
        facingRight = true; // Face right
        // Let update() handle animation state
    } else {
        // At target, stop moving
        velocityX = 0.0f;
        // Let update() handle animation state
    }
}

// Original render methods (use Game::renderer implicitly)
void Enemy::render() {
    if (!currentTexture) return;
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);
}

void Enemy::render(int x, int y) {
    if (!currentTexture) return;
    SDL_Rect tempDestRect = {x, y, destRect.w, destRect.h};
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &tempDestRect, 0, NULL, flip);
} 