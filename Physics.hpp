#pragma once
#include "GameObject.h" 
#include "Game.hpp" // For MAP_ROWS, MAP_COLS, TILE_SIZE
#include <algorithm> // Potentially needed by implementations

// Forward declare GameObject if needed, though likely included
// class GameObject;

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
    // Physics Constants
    static constexpr float GRAVITY = 0.5f;
    static constexpr float JUMP_FORCE = -13.86f; // For 6 tiles (192px)
    static constexpr float MAX_FALL_SPEED = 15.0f;
    static constexpr float FRICTION = 0.8f;
    // Add other constants if they existed, like GROUND_Y?
    // static constexpr int GROUND_Y = ???;

    // Collision Detection
    // Note: Using Game::MAP_COLS directly in the signature
    static bool checkCollisionSide(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int rows, int tileWidth, int tileHeight, int *collisionSide);
    
    // Physics Application
    static void applyGravity(GameObject* obj);
    static void applyFriction(GameObject* obj);
    static void applyJump(GameObject* obj); // May need modification depending on how jump was handled
    static bool isOnGround(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int rows, int tileWidth, int tileHeight);

    // Original movement function if it existed separately (might have been in GameObject)
    // static void moveWithPhysics(GameObject* obj, float dx, float dy);
}; 