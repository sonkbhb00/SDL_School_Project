#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Game.hpp"
#include <SDL.h>

enum CollisionSide {
    NONE,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

class Physics {
public:
    // Physics Constants (shared)
    static constexpr float GRAVITY = 0.5f;
    static constexpr float JUMP_FORCE = -10.0f;
    static constexpr float MAX_FALL_SPEED = 15.0f;
    static constexpr float FRICTION = 0.8f;
    static constexpr int GROUND_LEVEL = 555; // Fixed ground level at y = 495

    // Functions for GameObject
    static void applyGravity(GameObject* obj);
    static void applyFriction(GameObject* obj);
    static void applyJump(GameObject* obj);
    static bool isOnGround(const GameObject* obj);

    // Functions for Enemy
    static void applyGravity(Enemy* obj);
    static void applyFriction(Enemy* obj);
    static bool isOnGround(const Enemy* obj);
};
