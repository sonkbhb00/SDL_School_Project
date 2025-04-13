#include "GameObject.h"
#include "Game.hpp"
#include "Physics.hpp"
#include <SDL.h>
#include <iostream>
#include <cmath>   // For std::fabs
#include <cstdlib> // For std::rand and std::srand
#include <ctime>   // For std::time

GameObject::GameObject(int x, int y, int width, int height)
    : prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f),
      onGround(false),
      facingRight(true),
      currentState(IDLE),
      isAttacking(false),
      attackStartTime(0),
      attackDuration(300),
      isParrying(false),
      parryStartTime(0),
      parryDuration(300),
      inHitState(false),
      permanentlyDisabled(false),
      takeHitStartTime(0),
      takeHitDuration(300),
      xpos(x),
      ypos(y),
      currentFrame(0),
      lastFrameTime(SDL_GetTicks()),
      animSpeed(100),
      isMoving(false),
      animationTransitionThreshold(0.01f),
      parryFrameIndex(0)
{
    // Load textures
    idleTexture = TextureManager::loadTexture("assets/Idle.png");
    runTexture = TextureManager::loadTexture("assets/Run.png");
    attackTexture = TextureManager::loadTexture("assets/Attack.png");

    if (!idleTexture || !runTexture || !attackTexture) {
        std::cerr << "Failed to load textures" << std::endl;
        return;
    }

    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Start with idle animation
    currentTexture = idleTexture;
    totalFrames = 11; // Idle animation has 11 frames

    // Get sprite dimensions from idle texture
    int totalWidth, totalHeight;
    SDL_QueryTexture(idleTexture, NULL, NULL, &totalWidth, &totalHeight);
    frameHeight = totalHeight;
    frameWidth = totalWidth / totalFrames;

    // Set up rectangles
    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { x, y, static_cast<int>(frameWidth), static_cast<int>(frameHeight) };

    // Set up collider
    collider = { x, y, static_cast<int>(frameWidth / 2), static_cast<int>(frameHeight / 2) };
    collider.x = x + (destRect.w - collider.w) / 2;
    collider.y = y + (destRect.h - collider.h) / 2;
}

GameObject::~GameObject() {
    // Textures are managed by TextureManager
}

void GameObject::update() {
    if (inHitState) {
        if (SDL_GetTicks() - takeHitStartTime >= takeHitDuration) {
            inHitState = false;
            permanentlyDisabled = true;
            currentState = DEATH;
            velocityX = 0.0f;
        }
    }

    if (isAttacking) {
        if (SDL_GetTicks() - attackStartTime >= attackDuration) {
            isAttacking = false;
            if (!inHitState && !permanentlyDisabled) {
                currentState = onGround ? IDLE : JUMPING;
            }
        }
    }

    if (isParrying) {
        if (SDL_GetTicks() - parryStartTime >= parryDuration) {
            isParrying = false;
            if (!inHitState && !permanentlyDisabled) {
                currentTexture = idleTexture;
                currentFrame = 0;
                currentState = onGround ? IDLE : JUMPING;
            }
        } else {
            // Keep showing the selected parry frame
            currentFrame = parryFrameIndex;
        }
    }

    if (!isAttacking && !isParrying && !inHitState && !permanentlyDisabled) {
        if (!onGround && currentState != JUMPING) {
            currentState = JUMPING;
        } else if (onGround) {
            bool isMoving = std::fabs(velocityX) > 0.5f;
            currentState = isMoving ? RUNNING : IDLE;
        }
    }

    // Update animation frame
    Uint32 currentTime = SDL_GetTicks();
    if (!isParrying && currentTime - lastFrameTime >= static_cast<Uint32>(animSpeed)) {
        lastFrameTime = currentTime;

        // Get total frames based on current texture
        int maxFrames;
        if (currentTexture == idleTexture) maxFrames = 11;
        else if (currentTexture == runTexture) maxFrames = 8;
        else if (currentTexture == attackTexture) maxFrames = 6;
        else maxFrames = totalFrames;

        // Handle attack animation
        if (isAttacking) {
            if (currentFrame < maxFrames - 1) {
                currentFrame++;
            }
            // Only reset attack state when animation completes
            if (currentFrame >= maxFrames - 1) {
                isAttacking = false;
                currentTexture = idleTexture;
                currentFrame = 0;
                currentState = onGround ? IDLE : JUMPING;
                animSpeed = 100; // Reset animation speed
            }
        } else {
            currentFrame = (currentFrame + 1) % maxFrames;
        }

        srcRect.x = currentFrame * frameWidth;
    }

    // Handle movement animation transitions
    if (!isAttacking && !isParrying) {  // Don't change animations during attack or parry
        if (std::fabs(velocityX) > animationTransitionThreshold && !isMoving) {
            isMoving = true;
            currentTexture = runTexture;
            currentFrame = 0;
        } else if (std::fabs(velocityX) <= animationTransitionThreshold && isMoving) {
            if (currentFrame == 0 || currentFrame == 7) {
                isMoving = false;
                currentTexture = idleTexture;
                currentFrame = 0;
            }
        }
    }

    // Update rectangles
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

    // Draw character collider box (white outline)
    SDL_Rect tempCollider = {
        x + (destRect.w - collider.w) / 2,
        y + (destRect.h - collider.h) / 2,
        collider.w,
        collider.h
    };

    SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
    for (int i = 0; i < 3; i++) {
        SDL_Rect outline = tempCollider;
        outline.x -= i;
        outline.y -= i;
        outline.w += i * 2;
        outline.h += i * 2;
        SDL_RenderDrawRect(Game::renderer, &outline);
    }

    // Draw attack hitbox when attacking (red)
    if (isAttacking) {  // Changed from currentState == ATTACKING
        SDL_Rect attackBox = getAttackHitbox();
        attackBox.x = x + (attackBox.x - xpos);
        attackBox.y = y + (destRect.h - attackBox.h) / 2;

        SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, 255);
        for (int i = 0; i < 3; i++) {
            SDL_Rect outline = attackBox;
            outline.x -= i;
            outline.y -= i;
            outline.w += i * 2;
            outline.h += i * 2;
            SDL_RenderDrawRect(Game::renderer, &outline);
        }
    }

    SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
}

void GameObject::render() {
    renderSprite(xpos, ypos);
    renderHitboxes(xpos, ypos);
}

void GameObject::render(int x, int y) {
    renderSprite(x, y);
    renderHitboxes(x, y);
}

SDL_Rect GameObject::getAttackHitbox() const {
    if (!isAttacking) {  // Changed from currentState != ATTACKING
        return {0, 0, 0, 0};
    }

    SDL_Rect attackBox;
    attackBox.w = collider.w / 3.5;
    attackBox.h = collider.h;

    if (facingRight) {
        attackBox.x = collider.x + 50;
    } else {
        attackBox.x = collider.x;
    }

    attackBox.y = collider.y;
    return attackBox;
}

void GameObject::move(int dx) {
    float moveSpeed = 4.0f;

    if (!isAttacking && !isParrying && !inHitState && !permanentlyDisabled) {
        if (dx != 0) {
            velocityX = (dx > 0) ? moveSpeed : -moveSpeed;
            facingRight = (dx > 0);
        }
    }
}

void GameObject::takeHit() {
    if (!inHitState && !permanentlyDisabled) {
        inHitState = true;
        takeHitStartTime = SDL_GetTicks();
        currentState = TAKE_HIT;
        velocityX = 0.0f;
        velocityY = -2.0f;
        onGround = false;
    }
}

void GameObject::parry() {
    if (!isAttacking && !isParrying && !inHitState && !permanentlyDisabled) {
        isParrying = true;
        parryStartTime = SDL_GetTicks();
        currentTexture = attackTexture;

        // Randomly select one of the 6 attack frames (0-5)
        parryFrameIndex = std::rand() % 6;
        currentFrame = parryFrameIndex;

        // Set up the source rectangle for the selected frame
        srcRect.x = currentFrame * frameWidth;
    }
}

void GameObject::attack() {
    // Allow new attack if not already attacking, not parrying, not in hit state and not disabled
    // Removed additional isAttacking check since we want to allow repeated attacks
    if (!isParrying && !inHitState && !permanentlyDisabled) {
        isAttacking = true;
        attackStartTime = SDL_GetTicks();
        currentTexture = attackTexture;
        currentFrame = 0;
        animSpeed = 50;  // Make attack animation slightly faster
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
