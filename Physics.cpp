#include "Physics.hpp"
#include "GameObject.h"
#include "Game.hpp" // For constants
#include <SDL.h>
#include <cmath>   // For std::fabs
#include <algorithm> // For std::min/max

// Định nghĩa hằng số trọng lực và vị trí mặt đất
static constexpr float GRAVITY = 0.1f;
static constexpr int GROUND_Y = 310; // Vị trí y của mặt đất

bool Physics::checkCollisionSide(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int rows, int tileWidth, int tileHeight, int *collisionSide) {
    SDL_Rect playerRect = obj->getCollider(); // Use object's collider
    int cols = Game::MAP_COLS;

    // Calculate the range of tiles to check based on player position
    int leftTile = std::max(0, playerRect.x / tileWidth);
    int rightTile = std::min(cols - 1, (playerRect.x + playerRect.w) / tileWidth);
    int topTile = std::max(0, playerRect.y / tileHeight);
    int bottomTile = std::min(rows - 1, (playerRect.y + playerRect.h) / tileHeight);

    *collisionSide = NONE;

    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            // Check only solid tiles (assuming 0 and 1 are solid)
            if (mapMatrix[i][j] == 0 || mapMatrix[i][j] == 1) {
                SDL_Rect tileRect = { j * tileWidth, i * tileHeight, tileWidth, tileHeight };
                
                if (SDL_HasIntersection(&playerRect, &tileRect)) {
                    // Basic Penetration Check (similar to what was in GameObject)
                    // More precise side detection might be needed if this isn't sufficient
                    int penLeft = (playerRect.x + playerRect.w) - tileRect.x;
                    int penRight = (tileRect.x + tileRect.w) - playerRect.x;
                    int penTop = (playerRect.y + playerRect.h) - tileRect.y;
                    int penBottom = (tileRect.y + tileRect.h) - playerRect.y;
                    
                    int minPenX = (penLeft < penRight) ? -penLeft : penRight;
                    int minPenY = (penTop < penBottom) ? -penTop : penBottom;

                    if (std::abs(minPenX) < std::abs(minPenY)) {
                         *collisionSide = (minPenX < 0) ? LEFT : RIGHT;
                    } else {
                         *collisionSide = (minPenY < 0) ? TOP : BOTTOM;
                    }
                    return true; // Collision found
                }
            }
        }
    }
    return false; // No collision found
}

void Physics::applyGravity(GameObject* obj) {
    // Only apply if not flagged as on ground
    if (!obj->onGround) { 
        obj->velocityY += GRAVITY;
        // Limit fall speed
        if (obj->velocityY > MAX_FALL_SPEED) {
            obj->velocityY = MAX_FALL_SPEED;
        }
        // Update position (Vertical only)
        obj->ypos += static_cast<int>(obj->velocityY);
        // Update collider Y
        obj->destRect.y = obj->ypos;
    }
}

void Physics::applyFriction(GameObject* obj) {
    // Only apply friction when on ground
    if (obj->onGround) { 
        obj->velocityX *= FRICTION;
        if (std::fabs(obj->velocityX) < 0.1f) {
            obj->velocityX = 0.0f;
        }
    }
    
    // Always apply horizontal velocity to position, even in air
    obj->xpos += static_cast<int>(obj->velocityX);
    // Update collider X
    obj->destRect.x = obj->xpos;
}

void Physics::applyJump(GameObject* obj) {
    // Original simple jump logic
    if (obj->onGround) {
        obj->velocityY = JUMP_FORCE;
        obj->onGround = false;
    }
}

bool Physics::isOnGround(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int rows, int tileWidth, int tileHeight) {
    SDL_Rect collider = obj->getCollider();
    int cols = Game::MAP_COLS;
    
    // Add a small downward offset for the ground check sensor
    SDL_Rect groundSensor = { collider.x + 2, collider.y + collider.h, collider.w - 4, 2 }; 

    int leftTile = std::max(0, groundSensor.x / tileWidth);
    int rightTile = std::min(cols - 1, (groundSensor.x + groundSensor.w) / tileWidth);
    int topTile = std::max(0, groundSensor.y / tileHeight);
    int bottomTile = std::min(rows - 1, (groundSensor.y + groundSensor.h) / tileHeight);
    
    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i >= 0 && i < rows && j >= 0 && j < cols) {
                if (mapMatrix[i][j] == 0 || mapMatrix[i][j] == 1) { // Assuming 0 and 1 are solid ground
                    SDL_Rect tileRect = { j * tileWidth, i * tileHeight, tileWidth, tileHeight };
                    if (SDL_HasIntersection(&groundSensor, &tileRect)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
