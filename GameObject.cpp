#include "GameObject.h"
#include "TextureManager.h"
#include "Game.hpp"
#include "Physics.hpp"
#include "Combat.hpp"
#include <SDL.h>
#include <iostream>
#include <cstring> // Required for strcmp
#include <cmath>   // For std::fabs, std::fmod
#include <algorithm> // For std::min/max
#include <ctime>   // Required for std::time for seeding rng

// Helper function to load texture and get frame info
// Returns true on success, false on failure
bool GameObject::loadAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight) {
    texture = TextureManager::loadTexture(path);
    if (texture == nullptr) {
        std::cout << "Failed to load texture: " << path << std::endl;
        totalFrames = 0;
        frameWidth = 0;
        frameHeight = 0;
        return false;
    }

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameHeight = h; // Assume single row

    // Set exact frame counts for each animation type
    if (strcmp(path, "assets/Idle.png") == 0) {
        totalFrames = 11;  // Idle animation has exactly 11 frames
    } else if (strcmp(path, "assets/Run.png") == 0) {
        totalFrames = 8;   // Run animation has exactly 8 frames
    } else if (strcmp(path, "assets/Jump.png") == 0) {
        totalFrames = 4;   // Jump animation has exactly 4 frames
    } else if (strcmp(path, "assets/Attack.png") == 0) {
        totalFrames = 6;   // Attack animation has exactly 6 frames
    } else {
        totalFrames = 1;
        frameWidth = w;
        std::cout << "Warning: Unknown texture path \"" << path << "\", assuming 1 frame." << std::endl;
        return true;
    }

    frameWidth = w / totalFrames;
    return true;
}

GameObject::GameObject(const char* idleTexturePath, const char* runTexturePath, const char* jumpTexturePath, const char* attackTexturePath,
                       int x, int y, float scale)
    : prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f), 
      onGround(false),
      facingRight(true),
      destRect{},
      currentFrame(0),
      lastFrameTime(SDL_GetTicks()),
      currentState(IDLE),
      isAttacking(false),
      attackStartTime(0),
      attackDuration(300),
      isParrying(false),
      parryFrameIndex(0),
      parryStartTime(0),
      parryDuration(300),
      rng(static_cast<unsigned int>(std::time(0))),
      xpos(x),
      ypos(y),
      scale(scale),
      srcRect{},
      collider{},
      colliderOffsetX(0),  // Remove offset
      colliderOffsetY(0),  // Remove offset
      idleTexture(nullptr),
      runTexture(nullptr),
      jumpTexture(nullptr),
      attackTexture(nullptr),
      parryTexture(nullptr),
      currentTexture(nullptr),
      idleTotalFrames(0),
      runTotalFrames(0),
      jumpTotalFrames(0),
      attackTotalFrames(0),
      parryTotalFrames(0),
      currentTotalFrames(0),
      idleFrameWidth(0),
      idleFrameHeight(0),
      runFrameWidth(0),
      runFrameHeight(0),
      jumpFrameWidth(0),
      jumpFrameHeight(0),
      attackFrameWidth(0),
      attackFrameHeight(0),
      parryFrameWidth(0),
      parryFrameHeight(0),
      currentFrameWidth(0),
      currentFrameHeight(0),
      idleAnimSpeed(100),    // Faster idle animation (was 400)
      runAnimSpeed(80),     // Faster run animation (was 300)
      jumpAnimSpeed(150),   
      attackAnimSpeed(50),  
      parryAnimSpeed(80),
      currentAnimSpeed(100), // Match the initial idle speed
      completeRunAnimation(false)
{
    // Load textures and validate
    bool idleLoaded = loadAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight);
    if (!idleLoaded) {
        std::cerr << "Critical Error: Failed to load idle texture!" << std::endl;
        return;
    }

    // Initialize with idle animation
    currentTexture = idleTexture;
    currentTotalFrames = idleTotalFrames;
    currentFrameWidth = idleFrameWidth;
    currentFrameHeight = idleFrameHeight;
    currentAnimSpeed = idleAnimSpeed;

    // Set up initial rectangles
    destRect = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };
    srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };
    
    // Make collider match destRect exactly
    collider = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };

    // Load other animations
    loadAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    loadAnimationData(jumpTexturePath, jumpTexture, jumpTotalFrames, jumpFrameWidth, jumpFrameHeight);
    loadAnimationData(attackTexturePath, attackTexture, attackTotalFrames, attackFrameWidth, attackFrameHeight);
}

GameObject::~GameObject() {
    if (idleTexture) SDL_DestroyTexture(idleTexture);
    if (runTexture) SDL_DestroyTexture(runTexture);
    if (jumpTexture) SDL_DestroyTexture(jumpTexture); 
    if (attackTexture) SDL_DestroyTexture(attackTexture); // Clean up attack texture
}

void GameObject::setAnimation(AnimationState newState) {
    // Prevent resetting if already in the same state, unless it's Parry (needs specific frame setup)
    if (currentState == newState && currentTexture != nullptr && newState != PARRYING) return;

    switch (newState) {
        case IDLE:
            if (!idleTexture) return;
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            break;
        case RUNNING:
            if (!runTexture) return;
            currentTexture = runTexture;
            currentTotalFrames = runTotalFrames;
            currentFrameWidth = runFrameWidth;
            currentFrameHeight = runFrameHeight;
            currentAnimSpeed = runAnimSpeed;
            break;
        case JUMPING:
            if (!jumpTexture) return;
            currentTexture = jumpTexture;
            currentTotalFrames = jumpTotalFrames;
            currentFrameWidth = jumpFrameWidth;
            currentFrameHeight = jumpFrameHeight;
            currentAnimSpeed = jumpAnimSpeed;
            break;
        case ATTACKING:
            if (!attackTexture) return;
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames;
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            currentAnimSpeed = attackAnimSpeed;
            break;
        case PARRYING:
            if (!attackTexture) return; // Use attack texture for parry frames
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames; // Use total frames for consistency
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            // Parry doesn't use animation speed, it holds a frame
            currentAnimSpeed = 0; 
            break;
    }

    currentState = newState;
    // Reset frame only if not starting a parry (parry sets its own frame index)
    if (newState != PARRYING) {
       currentFrame = 0; 
    }
    lastFrameTime = SDL_GetTicks(); 
    
    // Set specific frame for Parry immediately
    if (currentState == PARRYING) {
        currentFrame = parryFrameIndex; 
        srcRect.x = parryFrameIndex * currentFrameWidth;
        srcRect.y = 0;
        srcRect.w = currentFrameWidth;
        srcRect.h = currentFrameHeight;
    } else {
        srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };
    }

    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
}

void GameObject::parry() {
    // Can only parry if not already attacking or parrying
    if (!isAttacking && !isParrying) { 
        isParrying = true;
        parryStartTime = SDL_GetTicks();
        
        // Select a random frame from the attack animation
        if (attackTotalFrames > 0) {
            std::uniform_int_distribution<int> dist(0, attackTotalFrames - 1);
            parryFrameIndex = dist(rng);
        } else {
            parryFrameIndex = 0; // Fallback if attack animation isn't loaded
        }

        setAnimation(PARRYING);
        std::cout << "Parry started. Frame: " << parryFrameIndex << ", Duration: " << parryDuration << "ms" << std::endl;
    }
}

void GameObject::attack() {
    // Can only attack if not already attacking or parrying
    if (!isAttacking && !isParrying) { 
        isAttacking = true;
        attackStartTime = SDL_GetTicks();
        // Make attack animation slightly faster (50ms per frame instead of previous timing)
        attackAnimSpeed = 50;
        attackDuration = static_cast<Uint32>(attackTotalFrames * attackAnimSpeed);
        setAnimation(ATTACKING);
        std::cout << "Attack started. Duration: " << attackDuration << "ms, Frames: " << attackTotalFrames << std::endl;
    }
}

void GameObject::move(int dx) {
    float moveSpeed = 4.0f;
    if (dx > 0) {
        velocityX = moveSpeed;
        facingRight = true;
        
        if (currentState != RUNNING && !isAttacking && !isParrying) {
            setAnimation(RUNNING);
        }
    } else if (dx < 0) {
        velocityX = -moveSpeed;
        facingRight = false;
        
        if (currentState != RUNNING && !isAttacking && !isParrying) {
            setAnimation(RUNNING);
        }
    } else {
        // If no movement input and we're running, go back to idle
        if (currentState == RUNNING && !isAttacking && !isParrying) {
            setAnimation(IDLE);
        }
    }
}

void GameObject::jump() {
    Physics::applyJump(this); 
}

void GameObject::revertPosition() { 
    xpos = prevX; 
    ypos = prevY; 
    destRect.x = xpos;
    destRect.y = ypos;
}

void GameObject::update() {
    Uint32 currentTime = SDL_GetTicks();

    // Handle parry state completion
    if (isParrying) {
        if (static_cast<int>(currentTime - parryStartTime) >= static_cast<int>(parryDuration)) {
            isParrying = false;
            setAnimation(IDLE);
        }
    }
    // Handle attack state completion and animation
    else if (isAttacking) {
        // Update attack animation frame
        Uint32 elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime >= static_cast<Uint32>(attackAnimSpeed)) {
            if (currentFrame < currentTotalFrames - 1) {
                currentFrame++;
                lastFrameTime = currentTime;
            } else if (currentTime - attackStartTime >= attackDuration) {
                isAttacking = false;
                setAnimation(IDLE);
            }
        }
    }
    // Normal animation state logic (only if not attacking or parrying)
    else {
        // First handle movement state transitions
        if (!onGround) {
            if (currentState != JUMPING) {
                setAnimation(JUMPING);
            }
        } else if (std::fabs(velocityX) > 0.1f) {
            if (currentState != RUNNING) {
                setAnimation(RUNNING);
            }
        } else {
            velocityX = 0.0f; // Ensure velocity is zeroed when idle
            if (currentState != IDLE) {
                setAnimation(IDLE);
            }
        }

        // Then handle animation frame updates
        Uint32 elapsedTime = currentTime - lastFrameTime;
        if (currentTotalFrames > 1 && elapsedTime >= static_cast<Uint32>(currentAnimSpeed)) {
            currentFrame = (currentFrame + 1) % currentTotalFrames;
            lastFrameTime = currentTime;
        }
    }

    // Always update source rectangle based on current frame
    srcRect.x = currentFrame * currentFrameWidth;
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;
    
    // Update destination rectangle size and position
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
    destRect.x = xpos;
    destRect.y = ypos;

    // Make collider half the size and center it
    collider.w = destRect.w / 2;
    collider.h = destRect.h / 2;
    collider.x = xpos + (destRect.w - collider.w) / 2;
    collider.y = ypos + (destRect.h - collider.h) / 2;
}

void GameObject::renderSprite(int x, int y) {
    if (!Game::renderer || !currentTexture) return;
    SDL_Rect tempDestRect = { x, y, destRect.w, destRect.h };
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &tempDestRect, 0, NULL, flip);
}

void GameObject::renderHitboxes(int x, int y) {
    if (!Game::renderer) return;
    
    // Draw character collider box with thicker lines (green)
    SDL_Rect tempCollider = {
        x + (destRect.w - collider.w) / 2, // Center horizontally
        y + (destRect.h - collider.h) / 2, // Center vertically
        collider.w,
        collider.h
    };
    
    SDL_SetRenderDrawColor(Game::renderer, 0, 255, 0, 255);
    // Draw thick lines by rendering multiple offset rectangles
    for (int i = 0; i < 3; i++) {
        SDL_Rect outline = tempCollider;
        outline.x -= i;
        outline.y -= i;
        outline.w += i * 2;
        outline.h += i * 2;
        SDL_RenderDrawRect(Game::renderer, &outline);
    }
    
    // Draw attack hitbox when attacking (red)
    if (currentState == ATTACKING) {
        SDL_Rect attackBox = getAttackHitbox();
        // Adjust for render position while maintaining centering
        attackBox.x = x + (attackBox.x - xpos);
        attackBox.y = y + (destRect.h - attackBox.h) / 2;
        
        SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, 255);
        // Draw thick lines for attack hitbox
        for (int i = 0; i < 3; i++) {
            SDL_Rect outline = attackBox;
            outline.x -= i;
            outline.y -= i;
            outline.w += i * 2;
            outline.h += i * 2;
            SDL_RenderDrawRect(Game::renderer, &outline);
        }
    }
    
    // Reset draw color
    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
}

// Update the original render methods to use the new split functions
void GameObject::render() {
    renderSprite(xpos, ypos);
    renderHitboxes(xpos, ypos);
}

void GameObject::render(int x, int y) {
    renderSprite(x, y);
    renderHitboxes(x, y);
}

SDL_Rect GameObject::getAttackHitbox() const {
    if (currentState != ATTACKING) {
        return {0, 0, 0, 0};
    }
    
    SDL_Rect attackBox;
    // Match the collider's half-size dimensions
    attackBox.w = collider.w / 3.5;
    attackBox.h = collider.h ;
    
    // Position the attack hitbox next to the collider
    if (facingRight) {
        attackBox.x = collider.x + 50; // Right of the player's hitbox
    } else {
        attackBox.x = collider.x; // Left of the player's hitbox
    }
    
    // Match the vertical position of the player's hitbox
    attackBox.y = collider.y;
    
    return attackBox;
}
