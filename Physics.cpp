#include "Physics.hpp"

bool Physics::checkCollisionWithMap(const GameObject *obj, const int mapMatrix[][50], int rows, int cols, int tileWidth, int tileHeight) {
    SDL_Rect collider = obj->getCollider();

    // Tính toán chỉ số các tile mà đối tượng chiếm dụng
    int leftTile   = collider.x / tileWidth;
    int rightTile  = (collider.x + collider.w) / tileWidth;
    int topTile    = collider.y / tileHeight;
    int bottomTile = (collider.y + collider.h) / tileHeight;

    // Duyệt qua các tile nằm trong vùng chiếm
    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i >= 0 && i < rows && j >= 0 && j < cols) {
                // Nếu tile có giá trị 0 hoặc 1, coi như có va chạm
                if (mapMatrix[i][j] == 0 || mapMatrix[i][j] == 1)
                    return true;
            }
        }
    }
    return false;
}
