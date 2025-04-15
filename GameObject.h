#pragma once
#include <SDL.h>
#include "Game.hpp"

class Physics;

enum AnimationState {
    IDLE,
    RUNNING,
    JUMPING,
    ATTACKING,
    PARRYING,
    DASHING,
    TAKE_HIT,
    DEATH
};

class GameObject {
public:
    GameObject(int x, int y, int width = 50, int height = 50);
    virtual ~GameObject();

    virtual void update();
    virtual void render();
    virtual void render(int x, int y);
    void renderSprite(int x, int y);
    void renderHitboxes(int x, int y);

    void move(int direction);
    void jump();
    void dash();
    void attack();
    void parry();
    void takeHit();

    // Accessors
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setX(int x) { xpos = x; destRect.x = x; collider.x = x + (destRect.w - collider.w) / 2; }
    void setY(int y) { ypos = y; destRect.y = y; collider.y = y + (destRect.h - collider.h) / 2; }
    AnimationState getState() const { return currentState; }
    void revertPosition();
    SDL_Rect getCollider() const { return collider; }
    SDL_Rect getAttackHitbox() const;
    void setGameRef(Game* gameRef) { game = gameRef; }
    int getCurrentFrame() const { return currentFrame; }
    bool isDeathAnimationComplete() const {
        return currentTexture == deathTexture && currentFrame >= 8;
    }
    void incrementEnemiesDefeated() { 
        enemiesDefeatedCount++; 
    }
    int getEnemiesDefeated() const { return enemiesDefeatedCount; }

    // Public members needed by Physics/Game - order matches constructor
    int prevX, prevY;
    float velocityX;
    float velocityY;
    int xpos, ypos;
    bool onGround;
    bool facingRight;
    
    // Animation and State variables
    AnimationState currentState;
    bool isAttacking;
    bool isParrying;
    bool isDashing;
    bool isInvincibleDuringDash;  // Added missing field
    bool hasParriedDuringDash;  // Track if already parried during this dash
    bool inHitState;
    bool permanentlyDisabled;
    bool isMoving;
    bool isFlashing;
    bool deathCountUpdated;
    
    // Timers and durations
    Uint32 attackStartTime;
    int attackDuration;
    Uint32 parryStartTime;
    Uint32 parryDuration;
    Uint32 dashStartTime;
    Uint32 takeHitStartTime;
    int takeHitDuration;
    Uint32 lastAttackTime;
    Uint32 lastParryTime;
    Uint32 lastDashTime;
    Uint32 flashStartTime;
    Uint32 flashDuration;
    Uint8 flashAlpha;

    SDL_Rect destRect;

    // Constants
    static const Uint32 DASH_DURATION = 250;  // 250ms dash
    static const Uint32 ATTACK_COOLDOWN = 500;
    static const Uint32 PARRY_COOLDOWN = 250;
    // Set dash speed to move exactly 50 pixels over the dash duration (250ms)
    static constexpr float DASH_SPEED = 200.0f; // 50 pixels / (250ms / 1000ms) = 200 pixels per second
    static const Uint32 BASE_DASH_COOLDOWN = 3000;  // 3 second base cooldown
    static const Uint32 MIN_ENEMIES_FOR_DASH = 15;  // Minimum enemies needed to dash
    static const Uint32 COOLDOWN_REDUCTION_PER_ENEMY = 500;  // 500ms reduction per enemy defeated

private:
    SDL_Rect srcRect;
    SDL_Rect collider;
    int currentFrame;
    int enemiesDefeatedCount;  // Track number of enemies defeated

    // Sprite animation properties
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* attackTexture;
    SDL_Texture* dashTexture;
    SDL_Texture* takeHitTexture;
    SDL_Texture* deathTexture;
    SDL_Texture* currentTexture;
    int totalFrames;
    int frameWidth;
    int frameHeight;
    static const int DASH_FRAMES = 4;

    // Animation state
    float animationTransitionThreshold;
    Uint32 lastFrameTime;
    int animSpeed;
    int parryFrameIndex;

    Game* game;

    friend class Physics;
};
