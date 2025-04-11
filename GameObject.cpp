#include "GameObject.h"
#include "TextureManager.h"
#include "Game.hpp"
#include "Physics.hpp"
#include <SDL.h>
#include <iostream>
#include <cstring> // Required for strcmp
#include <cmath>   // For std::fabs, std::fmod
#include <algorithm> // For std::min/max

// Helper function to load texture and get frame info
// Returns true on success, false on failure
bool loadAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight) {
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

    // **IMPORTANT: Hardcoded frame counts - MUST match your assets!**
    if (strcmp(path, "assets/Idle.png") == 0) {
        totalFrames = 11;
    } else if (strcmp(path, "assets/Run.png") == 0) {
        totalFrames = 8;
    } else if (strcmp(path, "assets/Jump.png") == 0) {
        totalFrames = 4; // Added Jump frame count
    } else {
        totalFrames = 1;
        std::cout << "Warning: Unknown texture path \"" << path << "\", assuming 1 frame." << std::endl;
    }

    if (totalFrames > 0) {
        frameWidth = w / totalFrames;
    } else {
        frameWidth = w;
        totalFrames = 1;
    }
    return true;
}

GameObject::GameObject(const char* idleTexturePath, const char* runTexturePath, const char* jumpTexturePath,
                       int x, int y, float scale, int idleAnimSpeed, int runAnimSpeed, int jumpAnimSpeed)
    : currentFrame(0), lastFrameTime(SDL_GetTicks()), currentState(IDLE), // Public states
      isAttacking(false), attackStartTime(0), attackDuration(0),
      onGround(false), velocityX(0.0f), velocityY(0.0f), facingRight(true),
      xpos(x), ypos(y), // Private members
      // prevX/Y implicitly initialized
      // srcRect/destRect initialized later
      scale(scale),
      // Private animation data
      idleTexture(nullptr), runTexture(nullptr), jumpTexture(nullptr), currentTexture(nullptr),
      idleAnimSpeed(idleAnimSpeed), runAnimSpeed(runAnimSpeed), jumpAnimSpeed(jumpAnimSpeed)
{
    // Other initializations happen here...
    prevX = x; // Explicitly init prev position
    prevY = y;
    // prevX/Y initialized implicitly or could be set here: prevX = x; prevY = y;
    // srcRect/destRect initialized later
    // Animation frame counts/dims/speed initialized later

    bool idleLoaded = loadAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight);
    bool runLoaded = loadAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    bool jumpLoaded = loadAnimationData(jumpTexturePath, jumpTexture, jumpTotalFrames, jumpFrameWidth, jumpFrameHeight);

    if (!idleLoaded && !runLoaded && !jumpLoaded) { // Check all textures
        std::cout << "Error: Failed to load any textures for GameObject!" << std::endl;
        destRect = {x, y, 0, 0};
        srcRect = {0, 0, 0, 0};
        return;
    }

    // Initialize to Idle state (or fallback)
    if (idleLoaded) {
        setAnimation(IDLE);
    } else if (runLoaded) {
        setAnimation(RUNNING);
    } else if (jumpLoaded) { // Should ideally have idle
         setAnimation(JUMPING);
    }

    destRect = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };

    std::cout << "GameObject created." << std::endl;
}

GameObject::~GameObject() {
    if (idleTexture) SDL_DestroyTexture(idleTexture);
    if (runTexture) SDL_DestroyTexture(runTexture);
    if (jumpTexture) SDL_DestroyTexture(jumpTexture); // Clean up jump texture
}

void GameObject::setAnimation(AnimationState newState) {
    if (currentState == newState && currentTexture != nullptr) return;

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
             if (!jumpTexture) return; // Don't switch if texture isn't loaded
            currentTexture = jumpTexture;
            currentTotalFrames = jumpTotalFrames;
            currentFrameWidth = jumpFrameWidth;
            currentFrameHeight = jumpFrameHeight;
            currentAnimSpeed = jumpAnimSpeed;
            break;
    }

    currentState = newState;
    currentFrame = 0;
    lastFrameTime = SDL_GetTicks();
    srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };

    // Update destRect height/width in case new animation has different dimensions
    // (Though likely they are the same, this is safer)
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);

    // std::cout << "Set animation state to: " << newState << std::endl; // Optional: Keep for debugging
}

void GameObject::move(int dx) {
    // Update horizontal velocity based on input direction
    // A simple implementation might just add a fixed amount, 
    // but a max speed cap is usually desirable.
    float moveSpeed = 4.0f; // Example speed, adjust as needed
    if (dx > 0) {
        velocityX = moveSpeed;
        facingRight = true;
    } else if (dx < 0) {
        velocityX = -moveSpeed;
        facingRight = false;
    } 
    // Note: We don't set velocityX to 0 when dx is 0 here.
    // Friction in Physics::applyFriction handles slowing down when there's no input.
    // We also don't change xpos directly. Physics::applyFriction will handle that.
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

    // Determine desired animation state based on member variables set by Physics
    AnimationState desiredState;
    if (!onGround) {
        desiredState = JUMPING;
    } else if (std::fabs(velocityX) > 0.1f) {
        desiredState = RUNNING;
    } else {
        desiredState = IDLE;
    }

    setAnimation(desiredState);

    if (currentTotalFrames > 1 && currentTime - lastFrameTime >= static_cast<Uint32>(currentAnimSpeed)) {
        currentFrame = (currentFrame + 1) % currentTotalFrames;
        lastFrameTime = currentTime;
    }

    srcRect.x = currentFrame * currentFrameWidth;
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;
    
    // Ensure destRect size is updated
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
}

void GameObject::render() {
    // Create a temporary rect with the current object position
    SDL_Rect destRect = {xpos, ypos, srcRect.w, srcRect.h};

    // Calculate index for current sprite
    int frameIndex = (currentFrame / currentAnimSpeed) % currentTotalFrames;
    // Move the source window to the correct spot to extract current frame
    srcRect.x = srcRect.w * frameIndex;
    srcRect.y = 0;

    // Flip based on facing direction
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    
    // Render with flipping (and still with source/dest rects for animations)
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);
}

void GameObject::render(int x, int y) {
    // Create a temporary rect with the provided position
    SDL_Rect destRect = {x, y, srcRect.w, srcRect.h};

    // Calculate index for current sprite
    int frameIndex = (currentFrame / currentAnimSpeed) % currentTotalFrames;
    // Move the source window to the correct spot to extract current frame
    srcRect.x = srcRect.w * frameIndex;
    srcRect.y = 0;

    // Flip based on facing direction
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    
    // Render with flipping (and still with source/dest rects for animations)
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);
}
