#pragma once
#include "GameObject.h"

class Physics {
public:
    static bool checkCollisionWithMap(const GameObject *obj, const int mapMatrix[][50], int rows, int cols, int tileWidth, int tileHeight);
    static void applyGravity(GameObject* obj);
};
