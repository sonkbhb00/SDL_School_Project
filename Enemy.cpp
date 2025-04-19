#include "Enemy.h"
#include "GameObject.h"
#include "TextureManager.h"
#include "Game.hpp"
#include "Physics.hpp"
#include <SDL.h>
#include <iostream>
#include <cstring>
#include <cmath>

bool Enemy::loadEnemyAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight) {
    texture = TextureManager::loadTexture(path);
    if (!texture) return false;

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameHeight = h;

    if (strcmp(path, "assets/Idle.png") == 0) totalFrames = 11;
    else if (strcmp(path, "assets/Run.png") == 0) totalFrames = 8;
    else if (strcmp(path, "assets/Attack.png") == 0) totalFrames = 6;
    else if (strcmp(path, "assets/Take Hit.png") == 0) totalFrames = 4;
    else if (strcmp(path, "assets/Death.png") == 0) totalFrames = 9;
    else totalFrames = 1;

    if (totalFrames > 0) frameWidth = w / totalFrames;
    else { frameWidth = w; totalFrames = 1; }
    return true;
}

Enemy::Enemy(const char* idleTexturePath, const char* runTexturePath, const char* attackTexturePath,
             const char* takeHitTexturePath, const char* deathTexturePath, int x, int y, float scale)
    : prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f),
      onGround(false),
      isInHitState(false),
      isPermanentlyDisabled(false),
      currentState(ENEMY_IDLE),
      currentFrame(0),
      xpos(x), ypos(y),
      scale(scale),
      colliderOffsetX(0), colliderOffsetY(0),
      facingRight(true),
      idleTexture(nullptr), runTexture(nullptr), attackTexture(nullptr),
      takeHitTexture(nullptr), deathTexture(nullptr), currentTexture(nullptr),
      isAttacking(false),
      attackStartTime(0),
      attackHitboxDuration(10),
      attackAnimDuration(300),
      attackRange(65.0f),
      lastAttackTime(0),
      attackCooldown(1000),
      idleTotalFrames(0), runTotalFrames(0), attackTotalFrames(0),
      takeHitTotalFrames(0), deathTotalFrames(0), currentTotalFrames(0),
      idleFrameWidth(0), idleFrameHeight(0), runFrameWidth(0), runFrameHeight(0),
      attackFrameWidth(0), attackFrameHeight(0),
      takeHitFrameWidth(0), takeHitFrameHeight(0), deathFrameWidth(0), deathFrameHeight(0),
      currentFrameWidth(0), currentFrameHeight(0),
      idleAnimSpeed(100),
      runAnimSpeed(80),
      attackAnimSpeed(100),
      takeHitAnimSpeed(100),
      deathAnimSpeed(150),
      currentAnimSpeed(100),
      lastFrameTime(SDL_GetTicks()),
      takeHitStartTime(0),
      takeHitDuration(300),
      isFlashing(false),
      flashStartTime(0),
      flashDuration(150),
      flashAlpha(255)
{
    // Load all textures with error checking
    if (!loadEnemyAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight) ||
        !loadEnemyAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight)) {
        std::cerr << "Error: Failed to load critical animations for Enemy!" << std::endl;
        return;
    }

    // Load optional animations
    if (!loadEnemyAnimationData(attackTexturePath, attackTexture, attackTotalFrames, attackFrameWidth, attackFrameHeight)) {
        std::cerr << "Warning: Failed to load attack animation" << std::endl;
    }
    if (!loadEnemyAnimationData(takeHitTexturePath, takeHitTexture, takeHitTotalFrames, takeHitFrameWidth, takeHitFrameHeight)) {
        std::cerr << "Warning: Failed to load take hit animation" << std::endl;
    }
    if (!loadEnemyAnimationData(deathTexturePath, deathTexture, deathTotalFrames, deathFrameWidth, deathFrameHeight)) {
        std::cerr << "Warning: Failed to load death animation" << std::endl;
    }

    // Initialize with idle animation
    currentTexture = idleTexture;
    currentTotalFrames = idleTotalFrames;
    currentFrameWidth = idleFrameWidth;
    currentFrameHeight = idleFrameHeight;
    currentAnimSpeed = idleAnimSpeed;

    // Set up rectangles
    destRect = { x, y, static_cast<int>(idleFrameWidth * scale), static_cast<int>(idleFrameHeight * scale) };
    srcRect = { 0, 0, idleFrameWidth, idleFrameHeight };

    // Set up collider as half the size of destRect
    collider.w = destRect.w / 2;
    collider.h = destRect.h / 2;
    collider.x = x + (destRect.w - collider.w) / 2;
    collider.y = y + (destRect.h - collider.h) / 2;
}

Enemy::~Enemy() {
    // Textures are handled by TextureManager
}

void Enemy::tryAttack(const GameObject* player) {
    // Don't attack if any of these conditions are true
    if (!player || isPermanentlyDisabled || isInHitState || isAttacking || player->permanentlyDisabled) return;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastAttackTime < attackCooldown) return;

    // Calculate distance to player
    float dx = player->getX() - xpos;
    float dy = player->getY() - ypos;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Update facing direction based on player position
    facingRight = dx > 0;

    // If within attack range and not currently attacking
    if (distance <= attackRange) {
        isAttacking = true;
        attackStartTime = currentTime;
        lastAttackTime = currentTime;
        setAnimation(ENEMY_ATTACKING);
        velocityX = 0.0f; // Stop moving while attacking
    }
}

void Enemy::update(const GameObject* player) {
    Uint32 currentTime = SDL_GetTicks();

    // Update flash effect
    if (isFlashing) {
        Uint32 flashElapsed = currentTime - flashStartTime;
        if (flashElapsed >= flashDuration) {
            isFlashing = false;
        } else {
            flashAlpha = static_cast<Uint8>(255 * (1.0f - static_cast<float>(flashElapsed) / flashDuration));
        }
    }

    // Try to attack if we have a valid player reference
    if (player && !isInHitState && !isPermanentlyDisabled) {
        tryAttack(player);
    }

    // Handle hit state and death transition
    if (isInHitState) {
        if (currentTime - takeHitStartTime >= takeHitDuration) {
            isInHitState = false;
            isPermanentlyDisabled = true;
            setAnimation(ENEMY_DEATH);
            currentFrame = 0;
            velocityX = 0.0f;
        }
    }

    // Handle attack state
    if (isAttacking) {
        Uint32 attackElapsed = currentTime - attackStartTime;

        // Update animation frames based on total animation duration
        int expectedFrame = (attackElapsed * attackTotalFrames) / attackAnimDuration;
        if (expectedFrame < attackTotalFrames && expectedFrame != currentFrame) {
            currentFrame = expectedFrame;
            lastFrameTime = currentTime;
        }

        // End attack animation when duration is exceeded
        if (attackElapsed >= attackAnimDuration) {
            isAttacking = false;
            if (!isInHitState && !isPermanentlyDisabled) {
                setAnimation(ENEMY_IDLE);
            }
        }
    }

    // Update animation state based on movement and conditions
    if (!isInHitState && !isAttacking && currentState != ENEMY_DEATH) {
        if (isPermanentlyDisabled) {
            setAnimation(ENEMY_DEATH);
        } else {
            bool isMoving = std::fabs(velocityX) > 0.1f;
            if (isMoving && currentState != ENEMY_RUNNING) {
                setAnimation(ENEMY_RUNNING);
            } else if (!isMoving && currentState != ENEMY_IDLE) {
                setAnimation(ENEMY_IDLE);
            }
        }
    }

    // Update animation frames for non-attack animations
    if (currentTexture && currentTotalFrames > 1 && !isAttacking) {
        Uint32 elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime >= static_cast<Uint32>(currentAnimSpeed)) {
            lastFrameTime = currentTime;

            if (currentState == ENEMY_DEATH) {
                if (currentFrame < currentTotalFrames - 1) {
                    currentFrame++;
                }
            } else if (currentState == ENEMY_TAKE_HIT) {
                if (currentFrame < currentTotalFrames - 1) {
                    currentFrame++;
                }
            } else {
                currentFrame = (currentFrame + 1) % currentTotalFrames;
            }
        }
    }

    // Update rectangles
    srcRect.x = currentFrame * currentFrameWidth;
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;

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

SDL_Rect Enemy::getAttackHitbox() const {
    // Only return a valid hitbox if we're attacking AND within the 10ms window
    if (currentState != ENEMY_ATTACKING ||
        SDL_GetTicks() - attackStartTime > attackHitboxDuration) {
        return {0, 0, 0, 0};
    }

    SDL_Rect attackBox;
    attackBox.w = collider.w / 3.5; // Match player's attack width
    attackBox.h = collider.h/2;

    if (facingRight) {
        attackBox.x = collider.x + 50; // Match player's 50 pixel forward attack
    } else {
        attackBox.x = collider.x; // Match player's 50 pixel forward attack
    }

    attackBox.y = collider.y;
    return attackBox;
}

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
        case ENEMY_ATTACKING:
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames;
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            currentAnimSpeed = attackAnimSpeed;
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

void Enemy::takeHit() {
    if (!isInHitState && !isPermanentlyDisabled) {
        isInHitState = true;
        takeHitStartTime = SDL_GetTicks();
        takeHitDuration = takeHitTotalFrames * takeHitAnimSpeed;
        if (takeHitDuration <= 0) takeHitDuration = 300;

        // Start flash effect
        isFlashing = true;
        flashStartTime = SDL_GetTicks();
        flashAlpha = 255;

        setAnimation(ENEMY_TAKE_HIT);

        // Add knockback effect based on the direction the enemy is facing
        float knockbackForce = 5.0f;
        velocityX = facingRight ? -knockbackForce : knockbackForce;
        velocityY = -4.0f; // Add a small upward force for visual effect
        onGround = false;
    }
}

void Enemy::aiMoveTowards(int targetX) {
    // Don't move if attacking, in hit state, or disabled
    if (isAttacking || isInHitState || isPermanentlyDisabled) {
        velocityX = 0.0f;
        return;
    }

    float moveSpeed = 3.0f;
    float distanceToTarget = targetX - xpos;
    float minMoveThreshold = 5.0f;

    if (std::abs(distanceToTarget) > minMoveThreshold) {
        velocityX = (distanceToTarget > 0) ? moveSpeed : -moveSpeed;
        facingRight = velocityX > 0;

        if (currentState != ENEMY_RUNNING) {
            setAnimation(ENEMY_RUNNING);
        }
    } else {
        velocityX = 0.0f;
        if (currentState != ENEMY_IDLE) {
            setAnimation(ENEMY_IDLE);
        }
    }
}

void Enemy::render() {
    if (!currentTexture) return;
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    // If flashing, set the blend mode and color mod
    if (isFlashing) {
        SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_ADD);
        SDL_SetTextureColorMod(currentTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(currentTexture, flashAlpha);
    }

    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);

    // Reset texture properties after rendering
    if (isFlashing) {
        SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureColorMod(currentTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(currentTexture, 255);
    }

    /*
    // Draw enemy collider box (blue)
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

    // Draw attack hitbox when attacking (red)
    if (currentState == ENEMY_ATTACKING) {
        SDL_Rect attackBox = getAttackHitbox();
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
    */
}

void Enemy::render(int x, int y) {
    if (!currentTexture) return;
    SDL_Rect tempDestRect = {x, y, destRect.w, destRect.h};
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    // If flashing, set the blend mode and color mod
    if (isFlashing) {
        SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_ADD);
        SDL_SetTextureColorMod(currentTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(currentTexture, flashAlpha);
    }

    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &tempDestRect, 0, NULL, flip);

    // Reset texture properties after rendering
    if (isFlashing) {
        SDL_SetTextureBlendMode(currentTexture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureColorMod(currentTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(currentTexture, 255);
    }

    /*
    // Draw enemy collider box (blue) with camera offset
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

    // Draw attack hitbox when attacking (red)
    if (currentState == ENEMY_ATTACKING) {
        SDL_Rect attackBox = getAttackHitbox();
        // Adjust attack box for camera position
        attackBox.x = x + (attackBox.x - xpos);
        attackBox.y = y + (attackBox.y - ypos);

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
    */
}
