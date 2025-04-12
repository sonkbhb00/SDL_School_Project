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

// Updated Constructor
Enemy::Enemy(const char* idleTexturePath, const char* runTexturePath, const char* takeHitTexturePath, const char* deathTexturePath,
             int x, int y, float scale)
    : prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f), 
      onGround(false),
      isInHitState(false),
      isPermanentlyDisabled(false),
      currentState(ENEMY_IDLE),
      currentFrame(0),
      xpos(x), ypos(y), 
      scale(scale),
      colliderOffsetX(0), colliderOffsetY(0), // Remove offsets for centered hitbox
      facingRight(true),
      idleTexture(nullptr), runTexture(nullptr), takeHitTexture(nullptr), deathTexture(nullptr), currentTexture(nullptr),
      idleTotalFrames(0), runTotalFrames(0), takeHitTotalFrames(0), deathTotalFrames(0), currentTotalFrames(0),
      idleFrameWidth(0), idleFrameHeight(0), runFrameWidth(0), runFrameHeight(0), 
      takeHitFrameWidth(0), takeHitFrameHeight(0), deathFrameWidth(0), deathFrameHeight(0),
      currentFrameWidth(0), currentFrameHeight(0),
      idleAnimSpeed(200), runAnimSpeed(150), takeHitAnimSpeed(100), deathAnimSpeed(150), currentAnimSpeed(200),
      lastFrameTime(SDL_GetTicks()),
      takeHitStartTime(0),
      takeHitDuration(300)
{
    // Load textures using original helper
    bool idleLoaded = loadEnemyAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight);
    bool runLoaded = loadEnemyAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    bool takeHitLoaded = loadEnemyAnimationData(takeHitTexturePath, takeHitTexture, takeHitTotalFrames, takeHitFrameWidth, takeHitFrameHeight);
    bool deathLoaded = loadEnemyAnimationData(deathTexturePath, deathTexture, deathTotalFrames, deathFrameWidth, deathFrameHeight);

    if (!idleLoaded && !runLoaded) { 
        std::cerr << "Error: Failed to load critical Idle/Run animations for Enemy!" << std::endl;
        return;
    }

    // Initialize with idle animation
    currentTexture = idleTexture;
    currentTotalFrames = idleTotalFrames;
    currentFrameWidth = idleFrameWidth;
    currentFrameHeight = idleFrameHeight;
    currentAnimSpeed = idleAnimSpeed;

    // Set up initial rectangles with centered collision box
    destRect = { x, y, static_cast<int>(idleFrameWidth * scale), static_cast<int>(idleFrameHeight * scale) };
    srcRect = { 0, 0, idleFrameWidth, idleFrameHeight };
    
    // Make collider match destRect exactly and center it
    collider.w = destRect.w;
    collider.h = destRect.h;
    collider.x = x + (destRect.w - collider.w) / 2;
    collider.y = y + (destRect.h - collider.h) / 2;
}

Enemy::~Enemy() {
    // Textures are handled by TextureManager
}

// Updated update method
void Enemy::update(const int mapData[][Game::MAP_COLS]) {
    Uint32 currentTime = SDL_GetTicks();

    // Handle hit state and death transition
    if (isInHitState) {
        if (currentTime - takeHitStartTime >= takeHitDuration) {
            isInHitState = false;
            isPermanentlyDisabled = true;
            setAnimation(ENEMY_DEATH);
            velocityX = 0.0f;
        }
    }

    // Update animation state based on movement and conditions
    if (!isInHitState && currentState != ENEMY_DEATH) {
        if (isPermanentlyDisabled) {
            if (currentState != ENEMY_IDLE) {
                setAnimation(ENEMY_IDLE);
            }
        } else if (!onGround) {
            if (currentState != ENEMY_IDLE) {
                setAnimation(ENEMY_IDLE);
            }
        } else if (std::fabs(velocityX) > 0.1f) {
            if (currentState != ENEMY_RUNNING) {
                setAnimation(ENEMY_RUNNING);
            }
            // Update facing direction based on velocity
            facingRight = velocityX > 0;
        } else {
            if (currentState != ENEMY_IDLE) {
                setAnimation(ENEMY_IDLE);
            }
        }
    }

    // Update animation frames with proper timing
    if (currentTexture && currentTotalFrames > 1) {
        Uint32 elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime >= static_cast<Uint32>(currentAnimSpeed)) {
            // For death and take hit animations, don't loop
            if (currentState == ENEMY_DEATH || currentState == ENEMY_TAKE_HIT) {
                if (currentFrame < currentTotalFrames - 1) {
                    currentFrame++;
                }
            } else {
                // For other animations, loop normally
                currentFrame = (currentFrame + 1) % currentTotalFrames;
            }
            lastFrameTime = currentTime;
        }
    }

    // Update source rectangle for rendering
    srcRect.x = currentFrame * currentFrameWidth;
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;

    // Update destination rectangle size
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
    destRect.x = xpos;
    destRect.y = ypos;

    // Make collider half size and center it, matching player's hitbox
    collider.w = destRect.w / 2;
    collider.h = destRect.h / 2;
    collider.x = xpos + (destRect.w - collider.w) / 2;
    collider.y = ypos + (destRect.h - collider.h) / 2;
}

// Updated setAnimation method
void Enemy::setAnimation(EnemyAnimationState newState) {
    if (currentState == newState) return;
    
    currentState = newState;
    currentFrame = 0;
    lastFrameTime = SDL_GetTicks();
    
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
            break;
        default:
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            break;
    }
}

// Updated takeHit method
void Enemy::takeHit() {
    if (!isInHitState) {
        isInHitState = true;
        takeHitStartTime = SDL_GetTicks();
        takeHitDuration = takeHitTotalFrames * takeHitAnimSpeed;
        if (takeHitDuration <= 0) takeHitDuration = 300;

        setAnimation(ENEMY_TAKE_HIT);
        velocityX = 0.0f;
        velocityY = -2.0f;
        onGround = false;
    }
}

// Updated aiMoveTowards method
void Enemy::aiMoveTowards(int targetX) {
    float moveSpeed = 2.0f;  // Slightly slower than player's speed
    
    if (targetX < xpos) {
        velocityX = -moveSpeed;
        facingRight = false;
        if (currentState != ENEMY_RUNNING && !isInHitState) {
            setAnimation(ENEMY_RUNNING);
        }
    } else if (targetX > xpos) {
        velocityX = moveSpeed;
        facingRight = true;
        if (currentState != ENEMY_RUNNING && !isInHitState) {
            setAnimation(ENEMY_RUNNING);
        }
    } else {
        velocityX = 0.0f;
        if (currentState != ENEMY_IDLE && !isInHitState) {
            setAnimation(ENEMY_IDLE);
        }
    }
}

// Updated render methods
void Enemy::render() {
    if (!currentTexture) return;
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);

    // Draw enemy hitbox (in blue) with proper centering
    SDL_Rect tempCollider = {
        xpos + (destRect.w - collider.w) / 2,
        ypos + (destRect.h - collider.h) / 2,
        collider.w,
        collider.h
    };

    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 255, 255);
    for (int i = 0; i < 3; i++) {
        SDL_Rect outline = tempCollider;
        outline.x -= i;
        outline.y -= i;
        outline.w += i * 2;
        outline.h += i * 2;
        SDL_RenderDrawRect(Game::renderer, &outline);
    }
    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
}

void Enemy::render(int x, int y) {
    if (!currentTexture) return;
    SDL_Rect tempDestRect = {x, y, destRect.w, destRect.h};
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &tempDestRect, 0, NULL, flip);

    // Draw enemy hitbox (in blue) with camera offset and proper centering
    SDL_Rect tempCollider = {
        x + (destRect.w - collider.w) / 2,
        y + (destRect.h - collider.h) / 2,
        collider.w,
        collider.h
    };

    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 255, 255);
    for (int i = 0; i < 3; i++) {
        SDL_Rect outline = tempCollider;
        outline.x -= i;
        outline.y -= i;
        outline.w += i * 2;
        outline.h += i * 2;
        SDL_RenderDrawRect(Game::renderer, &outline);
    }
    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
}