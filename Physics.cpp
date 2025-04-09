#include "Physics.hpp"
#include <SDL.h>

// Định nghĩa hằng số trọng lực và vị trí mặt đất
static constexpr float GRAVITY = 0.1f;
static constexpr int GROUND_Y = 310; // Vị trí y của mặt đất

bool Physics::checkCollisionWithMap(const GameObject *obj, const int mapMatrix[][50], int rows, int cols, int tileWidth, int tileHeight) {
    SDL_Rect collider = obj->getCollider();

    int leftTile   = collider.x / tileWidth;
    int rightTile  = (collider.x + collider.w) / tileWidth;
    int topTile    = collider.y / tileHeight;
    int bottomTile = (collider.y + collider.h) / tileHeight;

    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i >= 0 && i < rows && j >= 0 && j < cols) {
                if (mapMatrix[i][j] == 0 || mapMatrix[i][j] == 1)
                    return true;
            }
        }
    }
    return false;
}

void Physics::applyGravity(GameObject* obj) {
    // Nếu player chưa chạm mặt đất, áp dụng trọng lực
    if (obj->ypos + obj->destRect.h < GROUND_Y) {
        obj->velocityY += GRAVITY;
        obj->ypos += static_cast<int>(obj->velocityY);
    }
    // Nếu player đã chạm mặt đất, giữ nguyên vị trí và reset vận tốc
    else {
        obj->ypos = GROUND_Y - obj->destRect.h;
        obj->velocityY = 0.0f;
    }
}
