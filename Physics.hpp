#pragma once
// #include "GameObject.h" // Forward declare instead
// #include "Enemy.h" // Forward declare instead
#include "Game.hpp" // Need Game::MAP_COLS for array dimensions
#include <algorithm> // Potentially needed by implementations

// Forward declare classes
class GameObject;
class Enemy;

// Collision sides enum (if not already defined elsewhere)
enum CollisionSide {
    NONE = 0,
    TOP = 1,
    RIGHT = 2,
    BOTTOM = 3,
    LEFT = 4
};

class Physics {
public:
    // Physics Constants (shared)
    static constexpr float GRAVITY = 0.5f;
    static constexpr float JUMP_FORCE = -13.86f; // For 6 tiles (192px)
    static constexpr float MAX_FALL_SPEED = 15.0f;
    static constexpr float FRICTION = 0.8f;
    // Add other constants if they existed, like GROUND_Y?
    // static constexpr int GROUND_Y = ???;

    // --- Functions for GameObject --- 
    static bool checkCollisionSide(const GameObject *obj, const int (*mapMatrix)[Game::MAP_COLS], int tileWidth, int *collisionSide);
    static void applyGravity(GameObject* obj);
    static void applyFriction(GameObject* obj);
    static void applyJump(GameObject* obj);
    static bool isOnGround(const GameObject *obj, const int (*mapMatrix)[Game::MAP_COLS], int tileWidth);

    // --- Overloaded Functions for Enemy --- 
    static bool checkCollisionSide(const Enemy *obj, const int (*mapMatrix)[Game::MAP_COLS], int tileWidth, int *collisionSide);
    static void applyGravity(Enemy* obj);
    static void applyFriction(Enemy* obj);
    // static void applyJump(Enemy* obj); // Add if needed
    static bool isOnGround(const Enemy *obj, const int (*mapMatrix)[Game::MAP_COLS], int tileWidth);

    // Original movement function if it existed separately (might have been in GameObject)
    // static void moveWithPhysics(GameObject* obj, float dx, float dy);
}; 