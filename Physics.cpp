#include "Physics.hpp"
#include "GameObject.h"
#include "Enemy.h"
#include "Game.hpp"
#include <SDL.h>
#include <cmath>
#include <algorithm>

const int GROUND_LEVEL = 555;

void Physics::applyGravity(GameObject* obj) {
    if (obj) {
        if (!obj->onGround) {
            obj->velocityY += GRAVITY;
            if (obj->velocityY > MAX_FALL_SPEED) {
                obj->velocityY = MAX_FALL_SPEED;
            }
        }

        int nextY = obj->getY() + static_cast<int>(obj->velocityY);
        if (nextY + obj->destRect.h > GROUND_LEVEL) {
            obj->setY(GROUND_LEVEL - obj->destRect.h);
            obj->velocityY = 0;
            obj->onGround = true;
        } else {
            obj->setY(nextY);
            obj->onGround = false;
        }
    }
}

void Physics::applyFriction(GameObject* obj) {
    if (obj && obj->onGround && !obj->isDashing) {  // Don't apply friction during dash
        obj->velocityX *= FRICTION;
        if (std::abs(obj->velocityX) < 0.1f) {
            obj->velocityX = 0.0f;
        }
    }
    if (obj) {
        obj->setX(obj->getX() + static_cast<int>(obj->velocityX));
    }
}

void Physics::applyJump(GameObject* obj) {
    if (obj && obj->onGround) {
        obj->velocityY = JUMP_FORCE;
        obj->onGround = false;
    }
}

bool Physics::isOnGround(const GameObject* obj) {
    if (!obj) return false;
    return (obj->getY() + obj->destRect.h >= GROUND_LEVEL);
}

// Enemy physics implementations
void Physics::applyGravity(Enemy* obj) {
    if (obj) {
        if (!obj->onGround) {
            obj->velocityY += GRAVITY;
            if (obj->velocityY > MAX_FALL_SPEED) {
                obj->velocityY = MAX_FALL_SPEED;
            }
        }

        int nextY = obj->getY() + static_cast<int>(obj->velocityY);
        if (nextY + obj->destRect.h > GROUND_LEVEL) {
            obj->setY(GROUND_LEVEL - obj->destRect.h);
            obj->velocityY = 0;
            obj->onGround = true;
        } else {
            obj->setY(nextY);
            obj->onGround = false;
        }
    }
}

void Physics::applyFriction(Enemy* obj) {
    if (obj && obj->onGround) {
        obj->velocityX *= FRICTION;
        if (std::abs(obj->velocityX) < 0.1f) {
            obj->velocityX = 0.0f;
        }
    }
    if (obj) {
        obj->setX(obj->getX() + static_cast<int>(obj->velocityX));
    }
}

bool Physics::isOnGround(const Enemy* obj) {
    if (!obj) return false;
    return (obj->getY() + obj->destRect.h >= GROUND_LEVEL);
}
