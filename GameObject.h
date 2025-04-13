#pragma once
#include <SDL.h>
#include "Game.hpp" // Re-include if needed for Game::renderer implicitly
#include <vector>
#include <random> // For std::mt19937 and std::uniform_int_distribution
#include <ctime>   // For std::time

// Forward declare Physics class
class Physics;

// Enum for animation states
enum AnimationState {
    IDLE,
    RUNNING,
    JUMPING,
    ATTACKING,
    PARRYING,
    TAKE_HIT,
    DEATH
};

class GameObject {
public:
    // Original constructor signature
    GameObject(const char* idleTexturePath, const char* runTexturePath, const char* jumpTexturePath, 
              const char* attackTexturePath, const char* takeHitTexturePath, const char* deathTexturePath,
              int x, int y, float scale = 1.0f);
    virtual ~GameObject();

    virtual void update();
    // Original render signatures
    virtual void render(); 
    virtual void render(int x, int y);
    void renderSprite(int x, int y);  // New method to render just the sprite
    void renderHitboxes(int x, int y);  // New method to render just the hitboxes
    
    void move(int direction); // Simplified move
    void jump();
    void attack();
    void parry(); // Added Parry action
    void takeHit(); // Added Take Hit action

    // Accessors
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setX(int x) { 
        xpos = x; 
        destRect.x = x; 
        collider.x = x + (destRect.w - collider.w) / 2; 
    }
    void setY(int y) { 
        ypos = y; 
        destRect.y = y; 
        collider.y = y + (destRect.h - collider.h) / 2; 
    }
    AnimationState getState() const { return currentState; }
    void revertPosition();
    SDL_Rect getCollider() const { return collider; }
    SDL_Rect getAttackHitbox() const; // Calculate based on state and facing

    // Public members needed by Physics
    int prevX, prevY;
    float velocityX;
    float velocityY;
    bool onGround;
    bool facingRight;
    SDL_Rect destRect; // Destination rect for rendering

    // Public state variables (Consider making private)
    int currentFrame;
    Uint32 lastFrameTime;
    AnimationState currentState;

    // Attack state variables
    bool isAttacking;
    Uint32 attackStartTime;
    int attackDuration;

    // Parry state variables
    bool isParrying; // Flag for parry state
    int parryFrameIndex; // Which frame of attack anim to show
    Uint32 parryStartTime;
    Uint32 parryDuration; // How long to hold the parry pose
    
    // Random number generator for parry frame
    std::mt19937 rng; 

    // Hit and death state variables
    bool inHitState;
    bool permanentlyDisabled;
    Uint32 takeHitStartTime;
    Uint32 takeHitDuration;

    // Other getters
    bool getIsGrounded() const { return onGround; }
    bool isInHitState() const { return inHitState; }
    bool isPermanentlyDisabled() const { return permanentlyDisabled; }

    // Animation frame counters
    int getIdleFrameCount() const { return idleFrameCount; }
    int getRunFrameCount() const { return runFrameCount; }

private:
    int xpos, ypos;
    float scale;
    SDL_Rect srcRect;
    SDL_Rect collider; // Separate collider rect
    int colliderOffsetX, colliderOffsetY; // Offset from xpos, ypos

    // Animation related
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* jumpTexture;
    SDL_Texture* attackTexture;
    SDL_Texture* parryTexture; // Added Parry texture
    SDL_Texture* takeHitTexture; // Added Take Hit texture
    SDL_Texture* deathTexture; // Added Death texture
    SDL_Texture* currentTexture;
    int idleTotalFrames, runTotalFrames, jumpTotalFrames, attackTotalFrames, parryTotalFrames, takeHitTotalFrames, deathTotalFrames, currentTotalFrames;
    int idleFrameWidth, idleFrameHeight, runFrameWidth, runFrameHeight, jumpFrameWidth, jumpFrameHeight, attackFrameWidth, attackFrameHeight, parryFrameWidth, parryFrameHeight, takeHitFrameWidth, takeHitFrameHeight, deathFrameWidth, deathFrameHeight, currentFrameWidth, currentFrameHeight;
    int idleAnimSpeed, runAnimSpeed, jumpAnimSpeed, attackAnimSpeed, parryAnimSpeed, takeHitAnimSpeed, deathAnimSpeed, currentAnimSpeed;
    bool completeRunAnimation;

    // Animation frame counters
    int idleFrameCount;
    int runFrameCount;
    int maxIdleFrames;
    int maxRunFrames;

    // Private helper to switch animations
    void setAnimation(AnimationState newState);

    // Original texture loading helper signature
    bool loadAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight);

    // Friend declarations
    friend class Physics; 
};

// Forward declarations - Moved after GameObject class definition
namespace Combat {
    void attack(GameObject* player);
    void parry();
    void shoot();
    void poison();
    void reload();
}
