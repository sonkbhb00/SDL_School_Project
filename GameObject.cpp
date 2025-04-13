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

GameObject::GameObject(const char* idleTexturePath, const char* runTexturePath, 
    const char* jumpTexturePath, const char* attackTexturePath, 
    const char* takeHitTexturePath, const char* deathTexturePath,
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
      inHitState(false),
      permanentlyDisabled(false),
      takeHitStartTime(0),
      takeHitDuration(300),
      rng(static_cast<unsigned int>(std::time(0))),
      xpos(x),
      ypos(y),
      scale(scale),
      srcRect{},
      collider{},
      colliderOffsetX(0),
      colliderOffsetY(0),
      idleTexture(nullptr),
      runTexture(nullptr),
      jumpTexture(nullptr),
      attackTexture(nullptr),
      parryTexture(nullptr),
      takeHitTexture(nullptr),
      deathTexture(nullptr),
      currentTexture(nullptr),
      idleTotalFrames(0),
      runTotalFrames(0),
      jumpTotalFrames(0),
      attackTotalFrames(0),
      parryTotalFrames(0),
      takeHitTotalFrames(0),
      deathTotalFrames(0),
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
      takeHitFrameWidth(0),
      takeHitFrameHeight(0),
      deathFrameWidth(0),
      deathFrameHeight(0),
      currentFrameWidth(0),
      currentFrameHeight(0),
      idleAnimSpeed(100),
      runAnimSpeed(80),
      jumpAnimSpeed(80),
      attackAnimSpeed(50),
      parryAnimSpeed(80),
      takeHitAnimSpeed(100),
      deathAnimSpeed(150),
      currentAnimSpeed(100),
      completeRunAnimation(false),
      idleFrameCount(0),
      runFrameCount(0),
      maxIdleFrames(0),
      maxRunFrames(0)
{
    // Load required textures with validation
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
    collider = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };

    // Load other animations
    loadAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    loadAnimationData(jumpTexturePath, jumpTexture, jumpTotalFrames, jumpFrameWidth, jumpFrameHeight);
    loadAnimationData(attackTexturePath, attackTexture, attackTotalFrames, attackFrameWidth, attackFrameHeight);
    loadAnimationData(takeHitTexturePath, takeHitTexture, takeHitTotalFrames, takeHitFrameWidth, takeHitFrameHeight);
    loadAnimationData(deathTexturePath, deathTexture, deathTotalFrames, deathFrameWidth, deathFrameHeight);

    // Set max frames
    maxIdleFrames = idleTotalFrames;
    maxRunFrames = runTotalFrames;
}

GameObject::~GameObject() {
    // Clean up all textures
    if (idleTexture) SDL_DestroyTexture(idleTexture);
    if (runTexture) SDL_DestroyTexture(runTexture);
    if (jumpTexture) SDL_DestroyTexture(jumpTexture);
    if (attackTexture) SDL_DestroyTexture(attackTexture);
    if (parryTexture) SDL_DestroyTexture(parryTexture);
    if (takeHitTexture) SDL_DestroyTexture(takeHitTexture);
    if (deathTexture) SDL_DestroyTexture(deathTexture);
}

void GameObject::takeHit() {
    if (!inHitState && !permanentlyDisabled) {
        inHitState = true;
        takeHitStartTime = SDL_GetTicks();
        takeHitDuration = takeHitTotalFrames * takeHitAnimSpeed;
        if (takeHitDuration <= 0) takeHitDuration = 300;

        setAnimation(TAKE_HIT);
        velocityX = 0.0f;
        velocityY = -2.0f;
        onGround = false;
    }
}

void GameObject::setAnimation(AnimationState newState) {
    if (currentState == newState && currentTexture != nullptr && newState != PARRYING) {
        return;
    }

    AnimationState prevState = currentState;
    currentState = newState;

    switch(newState) {
        case IDLE:
            if (!idleTexture) return;
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            if (prevState != IDLE) {
                idleFrameCount = 0;
                currentFrame = 0;
            }
            break;
        case RUNNING:
            if (!runTexture) return;
            currentTexture = runTexture;
            currentTotalFrames = runTotalFrames;
            currentFrameWidth = runFrameWidth;
            currentFrameHeight = runFrameHeight;
            currentAnimSpeed = runAnimSpeed;
            if (prevState != RUNNING) {
                runFrameCount = 0;
                currentFrame = 0;
            }
            break;
        case JUMPING:
            if (!jumpTexture) return;
            currentTexture = jumpTexture;
            currentTotalFrames = jumpTotalFrames;
            currentFrameWidth = jumpFrameWidth;
            currentFrameHeight = jumpFrameHeight;
            currentAnimSpeed = jumpAnimSpeed;
            currentFrame = 0; // Always start jump from beginning
            break;
        case ATTACKING:
            if (!attackTexture) return;
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames;
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            currentAnimSpeed = attackAnimSpeed;
            currentFrame = 0; // Always start attack from beginning
            break;
        case PARRYING:
            if (!attackTexture) return;
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames;
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            currentAnimSpeed = parryAnimSpeed;
            break;
        case TAKE_HIT:
            if (!takeHitTexture) return;
            currentTexture = takeHitTexture;
            currentTotalFrames = takeHitTotalFrames;
            currentFrameWidth = takeHitFrameWidth;
            currentFrameHeight = takeHitFrameHeight;
            currentAnimSpeed = takeHitAnimSpeed;
            currentFrame = 0;
            break;
        case DEATH:
            if (!deathTexture) return;
            currentTexture = deathTexture;
            currentTotalFrames = deathTotalFrames;
            currentFrameWidth = deathFrameWidth;
            currentFrameHeight = deathFrameHeight;
            currentAnimSpeed = deathAnimSpeed;
            currentFrame = 0;
            break;
    }

    lastFrameTime = SDL_GetTicks();
    srcRect = { currentFrame * currentFrameWidth, 0, currentFrameWidth, currentFrameHeight };
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
    float minMoveThreshold = 0.5f;  // Threshold to prevent jittery state changes

    if (dx != 0 && !isAttacking && !isParrying) {
        // Smoothly adjust velocity
        float targetVelocity = dx > 0 ? moveSpeed : -moveSpeed;
        velocityX = targetVelocity;
        facingRight = dx > 0;

        // Only switch to running if we're on the ground and moving fast enough
        if (onGround && std::fabs(velocityX) > minMoveThreshold) {
            if (currentState != RUNNING) {
                setAnimation(RUNNING);
            }
        }
    } else if (!isAttacking && !isParrying) {
        // Smoothly decelerate to stop
        velocityX = 0.0f;
        
        // Only switch to idle when we've actually stopped and we're on the ground
        if (onGround && std::fabs(velocityX) < minMoveThreshold) {
            if (currentState != IDLE) {
                setAnimation(IDLE);
            }
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

    // Handle hit state and death transition
    if (inHitState) {
        if (currentTime - takeHitStartTime >= takeHitDuration) {
            inHitState = false;
            permanentlyDisabled = true;
            setAnimation(DEATH);
            velocityX = 0.0f;
        }
    }

    // Reset attack state when animation completes
    if (isAttacking) {
        if (currentTime - attackStartTime >= attackDuration) {
            isAttacking = false;
            if (!inHitState && !permanentlyDisabled) {
                setAnimation(onGround ? IDLE : JUMPING);
            }
        }
    }

    // State transitions
    if (!isParrying && !isAttacking && !inHitState && !permanentlyDisabled) {
        if (!onGround && currentState != JUMPING) {
            setAnimation(JUMPING);
        } else if (onGround) {
            bool isMoving = std::fabs(velocityX) > 0.5f;
            if (isMoving && currentState != RUNNING) {
                setAnimation(RUNNING);
            } else if (!isMoving && currentState != IDLE) {
                setAnimation(IDLE);
            }
        }
    }

    // Handle frame updates for all states
    if (currentTexture && currentTotalFrames > 1) {
        Uint32 elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime >= static_cast<Uint32>(currentAnimSpeed)) {
            lastFrameTime = currentTime;
            
            if (currentState == DEATH || currentState == TAKE_HIT) {
                if (currentFrame < currentTotalFrames - 1) {
                    currentFrame++;
                }
            } else {
                currentFrame = (currentFrame + 1) % currentTotalFrames;
            }
        }
    }

    // Update visual state with frame validation
    srcRect.x = (currentFrame % currentTotalFrames) * currentFrameWidth;
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;

    // Update position and size
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
    destRect.x = xpos;
    destRect.y = ypos;

    // Update collider
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
