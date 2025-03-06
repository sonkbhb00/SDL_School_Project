#pragma once
#include <SDL.h>
#include "GameObject.h"

class Physics {
public:
    // Kiểm tra va chạm giữa 2 hình chữ nhật (bounding box)
    static bool checkCollision(const SDL_Rect &a, const SDL_Rect &b) {
        if (a.x + a.w <= b.x) return false;
        if (a.x >= b.x + b.w) return false;
        if (a.y + a.h <= b.y) return false;
        if (a.y >= b.y + b.h) return false;
        return true;
    }

    // Kiểm tra va chạm giữa 2 GameObject (sử dụng bounding box)
    static bool checkCollision(const GameObject &obj1, const GameObject &obj2) {
        return checkCollision(obj1.getCollider(), obj2.getCollider());
    }

    // Kiểm tra va chạm của một đối tượng với map dựa trên giá trị của tile
    // mapMatrix: mảng 2D chứa dữ liệu map (giá trị 0 đại diện cho tile có va chạm)
    // rows, cols: số hàng và số cột của map
    // tileWidth, tileHeight: kích thước mỗi tile tính theo pixel
    static bool checkCollisionWithMap(const GameObject *obj, const int mapMatrix[][50], int rows, int cols, int tileWidth, int tileHeight);


};
