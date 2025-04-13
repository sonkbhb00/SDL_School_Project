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

    // Public members needed by Physics
    int prevX, prevY;
    float velocityX;
    float velocityY;
    bool onGround;
    bool facingRight;
    SDL_Rect destRect;

    // State variables
    AnimationState currentState;
    bool isAttacking;
    Uint32 attackStartTime;
    int attackDuration;
    bool isParrying;
    Uint32 parryStartTime;
    Uint32 parryDuration;
    bool inHitState;
    bool permanentlyDisabled;
    Uint32 takeHitStartTime;
    Uint32 takeHitDuration;

private:
    int xpos, ypos;
    SDL_Rect srcRect;
    SDL_Rect collider;

    // Sprite animation properties
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* attackTexture;
    SDL_Texture* currentTexture;
    int totalFrames;
    int frameWidth;
    int frameHeight;
    int currentFrame;
    Uint32 lastFrameTime;
    int animSpeed;

    // Animation state
    bool isMoving;
    float animationTransitionThreshold;

    // Parry state
    int parryFrameIndex;  // The randomly selected frame for parry animation

    friend class Physics;
};
