#include "Physics.hpp"
#include "GameObject.h"
#include "Enemy.h"
#include "Game.hpp" // Include for Game constants
#include <SDL.h>
#include <cmath>   // For std::fabs
#include <algorithm> // For std::min/max

// Định nghĩa hằng số trọng lực và vị trí mặt đất
static constexpr float GRAVITY = 0.1f;
static constexpr int GROUND_Y = 310; // Vị trí y của mặt đất

// Updated signature: takes mapCols, uses const int**
// Updated signature: uses int(*)[Game::MAP_COLS], removed rows, tileHeight params
bool Physics::checkCollisionSide(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int tileWidth, int *collisionSide) {
    if (!obj || !mapMatrix || !collisionSide) return false;
    *collisionSide = NONE;
    SDL_Rect collider = obj->getCollider();
    int mapCols = Game::MAP_COLS; // Use Game constant
    int mapRows = Game::MAP_ROWS; // Use Game constant
    int tileHeight = Game::TILE_SIZE; // Use Game constant

    // Calculate tile coordinates spanned by the object
    int startCol = collider.x / tileWidth;
    int endCol = (collider.x + collider.w - 1) / tileWidth;
    int startRow = collider.y / tileHeight;
    int endRow = (collider.y + collider.h - 1) / tileHeight;

    // Clamp coordinates
    startCol = std::max(0, std::min(startCol, mapCols - 1));
    endCol = std::max(0, std::min(endCol, mapCols - 1));
    startRow = std::max(0, std::min(startRow, mapRows - 1)); // Use mapRows
    endRow = std::max(0, std::min(endRow, mapRows - 1));   // Use mapRows

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (mapMatrix[row][col] != 0) { 
                SDL_Rect tileRect = { col * tileWidth, row * tileHeight, tileWidth, tileHeight };
                if (SDL_HasIntersection(&collider, &tileRect)) {
                    // Determine collision side logic (as it was before)
                    int overlapLeft = (collider.x + collider.w) - tileRect.x;
                    int overlapRight = (tileRect.x + tileWidth) - collider.x;
                    int overlapTop = (collider.y + collider.h) - tileRect.y;
                    int overlapBottom = (tileRect.y + tileHeight) - collider.y;
                    int minOverlapX = std::min(overlapLeft, overlapRight);
                    int minOverlapY = std::min(overlapTop, overlapBottom);
                    if (minOverlapY < minOverlapX) {
                        if (obj->prevY + collider.h <= tileRect.y) *collisionSide = BOTTOM;
                        else if (obj->prevY >= tileRect.y + tileHeight) *collisionSide = TOP;
                    } else {
                        if (obj->prevX + collider.w <= tileRect.x) *collisionSide = RIGHT;
                        else if (obj->prevX >= tileRect.x + tileWidth) *collisionSide = LEFT;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

void Physics::applyGravity(GameObject* obj) {
    if (obj) {
        obj->velocityY += GRAVITY;
        if (obj->velocityY > MAX_FALL_SPEED) obj->velocityY = MAX_FALL_SPEED;
        obj->setY(obj->getY() + static_cast<int>(obj->velocityY));
    }
}

void Physics::applyFriction(GameObject* obj) {
    if (obj && obj->onGround) {
        obj->velocityX *= FRICTION;
        if (std::abs(obj->velocityX) < 0.1f) obj->velocityX = 0.0f;
    }
    // Apply horizontal velocity regardless of ground status
    if (obj) obj->setX(obj->getX() + static_cast<int>(obj->velocityX));
}

void Physics::applyJump(GameObject* obj) {
    if (obj && obj->onGround) {
        obj->velocityY = JUMP_FORCE;
        obj->onGround = false;
    }
}

// Updated signature: uses int(*)[Game::MAP_COLS], removed rows, tileHeight params
bool Physics::isOnGround(const GameObject *obj, const int mapMatrix[][Game::MAP_COLS], int tileWidth) {
    if (!obj || !mapMatrix) return false;
    SDL_Rect collider = obj->getCollider();
    int mapCols = Game::MAP_COLS;
    int mapRows = Game::MAP_ROWS; // Use Game constant
    int tileHeight = Game::TILE_SIZE; // Use Game constant
    int checkY = collider.y + collider.h; 
    int startCol = collider.x / tileWidth;
    int endCol = (collider.x + collider.w - 1) / tileWidth;
    int checkRow = checkY / tileHeight;

    // Clamp coordinates
    startCol = std::max(0, std::min(startCol, mapCols - 1));
    endCol = std::max(0, std::min(endCol, mapCols - 1));
    checkRow = std::max(0, std::min(checkRow, mapRows - 1)); // Use mapRows

    for (int col = startCol; col <= endCol; ++col) {
        if (mapMatrix[checkRow][col] != 0) {
             int tileTop = checkRow * tileHeight;
             if (checkY >= tileTop && checkY < tileTop + tileHeight) return true;
        }
    }
    return false;
}

// Updated signature: uses int(*)[Game::MAP_COLS], removed rows, tileHeight params
bool Physics::checkCollisionSide(const Enemy *obj, const int mapMatrix[][Game::MAP_COLS], int tileWidth, int *collisionSide) {
    // Logic identical to GameObject version
    if (!obj || !mapMatrix || !collisionSide) return false;
    *collisionSide = NONE;
    SDL_Rect collider = obj->getCollider();
    int mapCols = Game::MAP_COLS;
    int mapRows = Game::MAP_ROWS; // Use Game constant
    int tileHeight = Game::TILE_SIZE; // Use Game constant
    int startCol = collider.x / tileWidth;
    int endCol = (collider.x + collider.w - 1) / tileWidth;
    int startRow = collider.y / tileHeight;
    int endRow = (collider.y + collider.h - 1) / tileHeight;
    startCol = std::max(0, std::min(startCol, mapCols - 1));
    endCol = std::max(0, std::min(endCol, mapCols - 1));
    startRow = std::max(0, std::min(startRow, mapRows - 1)); // Use mapRows
    endRow = std::max(0, std::min(endRow, mapRows - 1));   // Use mapRows
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (mapMatrix[row][col] != 0) {
                SDL_Rect tileRect = { col * tileWidth, row * tileHeight, tileWidth, tileHeight };
                if (SDL_HasIntersection(&collider, &tileRect)) {
                    int overlapLeft = (collider.x + collider.w) - tileRect.x;
                    int overlapRight = (tileRect.x + tileWidth) - collider.x;
                    int overlapTop = (collider.y + collider.h) - tileRect.y;
                    int overlapBottom = (tileRect.y + tileHeight) - collider.y;
                    int minOverlapX = std::min(overlapLeft, overlapRight);
                    int minOverlapY = std::min(overlapTop, overlapBottom);
                    if (minOverlapY < minOverlapX) {
                        if (obj->prevY + collider.h <= tileRect.y) *collisionSide = BOTTOM;
                        else if (obj->prevY >= tileRect.y + tileHeight) *collisionSide = TOP;
                    } else {
                        if (obj->prevX + collider.w <= tileRect.x) *collisionSide = RIGHT;
                        else if (obj->prevX >= tileRect.x + tileWidth) *collisionSide = LEFT;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

void Physics::applyGravity(Enemy* obj) {
    if (obj) {
        obj->velocityY += GRAVITY;
        if (obj->velocityY > MAX_FALL_SPEED) obj->velocityY = MAX_FALL_SPEED;
        obj->setY(obj->getY() + static_cast<int>(obj->velocityY));
    }
}

void Physics::applyFriction(Enemy* obj) {
    if (obj && obj->onGround) {
        obj->velocityX *= FRICTION;
        if (std::abs(obj->velocityX) < 0.1f) obj->velocityX = 0.0f;
    }
    if (obj) obj->setX(obj->getX() + static_cast<int>(obj->velocityX));
}

// Updated signature: uses int(*)[Game::MAP_COLS], removed rows, tileHeight params
bool Physics::isOnGround(const Enemy *obj, const int mapMatrix[][Game::MAP_COLS], int tileWidth) {
    // Logic identical to GameObject version
    if (!obj || !mapMatrix) return false;
    SDL_Rect collider = obj->getCollider();
    int mapCols = Game::MAP_COLS;
    int mapRows = Game::MAP_ROWS; // Use Game constant
    int tileHeight = Game::TILE_SIZE; // Use Game constant
    int checkY = collider.y + collider.h;
    int startCol = collider.x / tileWidth;
    int endCol = (collider.x + collider.w - 1) / tileWidth;
    int checkRow = checkY / tileHeight;
    startCol = std::max(0, std::min(startCol, mapCols - 1));
    endCol = std::max(0, std::min(endCol, mapCols - 1));
    checkRow = std::max(0, std::min(checkRow, mapRows - 1)); // Use mapRows
    for (int col = startCol; col <= endCol; ++col) {
        if (mapMatrix[checkRow][col] != 0) {
             int tileTop = checkRow * tileHeight;
             if (checkY >= tileTop && checkY < tileTop + tileHeight) return true;
        }
    }
    return false;
}
