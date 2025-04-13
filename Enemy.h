#pragma once
#include <SDL.h>
#include "Game.hpp"
#include "TextureManager.h"
#include "Physics.hpp"
#include <string>
#include <vector>
#include <iostream>

class Physics;

enum EnemyAnimationState {
    ENEMY_IDLE,
    ENEMY_RUNNING,
    ENEMY_JUMPING,
    ENEMY_ATTACKING,
    ENEMY_TAKE_HIT,
    ENEMY_DEATH 
};

class Enemy {
public:
    Enemy(const char* idleTexturePath, const char* runTexturePath, const char* attackTexturePath,
          const char* takeHitTexturePath, const char* deathTexturePath, 
          int x, int y, float scale = 1.0f);
    ~Enemy();

    void update(const GameObject* player); // Changed from update(const int mapData[][Game::MAP_COLS])
    void render();
    void render(int x, int y);

    // Basic getters and setters
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setX(int x) { xpos = x; destRect.x = x; collider.x = x + colliderOffsetX; }
    void setY(int y) { ypos = y; destRect.y = y; collider.y = y + colliderOffsetY; }
    void revertPosition() { setX(prevX); setY(prevY); }
    SDL_Rect getCollider() const { return collider; }
    bool isTakingHit() const { return isInHitState; }
    void takeHit();
    void aiMoveTowards(int targetX);
    void setState(EnemyAnimationState newState) { setAnimation(newState); }
    EnemyAnimationState getState() const { return currentState; }
    SDL_Rect getAttackHitbox() const;

    // Physics properties
    int prevX, prevY;
    float velocityX, velocityY;
    bool onGround;
    bool isInHitState;
    bool isPermanentlyDisabled;
    
    SDL_Rect destRect;
    EnemyAnimationState currentState;
    int currentFrame;

private:
    int xpos, ypos;
    float scale;
    SDL_Rect srcRect;
    SDL_Rect collider;
    int colliderOffsetX, colliderOffsetY;
    bool facingRight;

    // Animation properties
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* attackTexture;
    SDL_Texture* takeHitTexture;
    SDL_Texture* deathTexture;
    SDL_Texture* currentTexture;

    // Attack properties
    bool isAttacking;
    Uint32 attackStartTime;
    Uint32 attackDuration;
    float attackRange;
    Uint32 lastAttackTime;
    Uint32 attackCooldown;

    int idleTotalFrames, runTotalFrames, attackTotalFrames, takeHitTotalFrames, deathTotalFrames, currentTotalFrames;
    int idleFrameWidth, idleFrameHeight, runFrameWidth, runFrameHeight;
    int attackFrameWidth, attackFrameHeight;
    int takeHitFrameWidth, takeHitFrameHeight, deathFrameWidth, deathFrameHeight;
    int currentFrameWidth, currentFrameHeight;
    int idleAnimSpeed, runAnimSpeed, attackAnimSpeed, takeHitAnimSpeed, deathAnimSpeed, currentAnimSpeed;

    Uint32 lastFrameTime;
    Uint32 takeHitStartTime;
    Uint32 takeHitDuration;

    // Hit flash effect properties
    bool isFlashing;
    Uint32 flashStartTime;
    Uint32 flashDuration;
    Uint8 flashAlpha;
    
    bool loadEnemyAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight);
    void setAnimation(EnemyAnimationState newState);
    void tryAttack(const GameObject* player);
    
    friend class Physics;
    friend class Game;
};