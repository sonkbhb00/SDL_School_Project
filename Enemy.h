#pragma once
#include <SDL.h>
#include "Game.hpp" // Re-include
#include "TextureManager.h" // Include necessary headers
#include "Physics.hpp" // Include Physics header
#include <string>
#include <vector>
#include <iostream>

// Forward declare classes
class Physics;
// class Game; // No longer needed if Game.hpp is included

// Basic animation states for Enemy
enum EnemyAnimationState {
    ENEMY_IDLE,
    ENEMY_RUNNING,
    ENEMY_JUMPING, // Assuming enemy might jump?
    ENEMY_TAKE_HIT,
    ENEMY_DEATH 
};

class Enemy {
public:
    // Original constructor
    Enemy(const char* idleTexturePath, const char* runTexturePath, const char* takeHitTexturePath, const char* deathTexturePath, 
          int x, int y, float scale = 1.0f, int idleAnimSpeed = 150, int runAnimSpeed = 100, int takeHitAnimSpeed = 100, int deathAnimSpeed = 150);
    ~Enemy();

    // Original update signature
    void update(const int mapData[][Game::MAP_COLS]); 
    // Original render signatures
    void render();
    void render(int x, int y);

    // Basic getters
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setX(int x) { xpos = x; destRect.x = x; } // Setter needed for collision response
    void setY(int y) { ypos = y; destRect.y = y; } // Setter needed for collision response
    void revertPosition() { setX(prevX); setY(prevY); }
    SDL_Rect getCollider() const { return destRect; }

    // Method to trigger the hit reaction
    void takeHit();
    
    // Check if enemy is currently taking a hit
    bool isTakingHit() const { return isInHitState; }
    
    // AI movement method - make it public
    void aiMoveTowards(int targetX);
    
    // Add a method to set the animation state
    void setState(EnemyAnimationState newState) { setAnimation(newState); }
    
    // Get current animation state
    EnemyAnimationState getState() const { return currentState; }

    // Public members needed by Physics or Game update loop for collision
    int prevX, prevY;
    float velocityX, velocityY;
    bool onGround;
    bool isInHitState; // Flag to check if *currently* reacting to a hit
    bool isPermanentlyDisabled; // Flag to track if enemy should stop moving forever
    
    // Make animation state accessible
    EnemyAnimationState currentState;
    int currentFrame;
    int deathTotalFrames;

private:
    int xpos, ypos;
    float scale;
    SDL_Rect srcRect, destRect;
    bool facingRight;

    // Animation Data
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* takeHitTexture;
    SDL_Texture* deathTexture; 
    SDL_Texture* currentTexture;

    int idleTotalFrames, runTotalFrames, takeHitTotalFrames, currentTotalFrames;
    int idleFrameWidth, idleFrameHeight, runFrameWidth, runFrameHeight, takeHitFrameWidth, takeHitFrameHeight, deathFrameWidth, deathFrameHeight, currentFrameWidth, currentFrameHeight;
    int idleAnimSpeed, runAnimSpeed, takeHitAnimSpeed, deathAnimSpeed, currentAnimSpeed;

    Uint32 lastFrameTime;

    // Hit state timing
    Uint32 takeHitStartTime;
    Uint32 takeHitDuration;

    // Original helper signature
    bool loadEnemyAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight);
    
    // Helper to switch animations
    void setAnimation(EnemyAnimationState newState);
    
    friend class Physics; // Allow Physics class access
    friend class Game; // Add back if needed
}; 