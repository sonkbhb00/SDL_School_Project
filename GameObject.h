#pragma once
#include "game.hpp"

class GameObject {
public:
    GameObject(const char* texture, int x, int y, int totalFrames = 1, int totalRows = 1, int animSpeed = 100);
    ~GameObject();

    void Update();
    void Render();
    void Move(int dx, int dy);
    void revertPosition();
    void Animate();

    SDL_Rect getCollider() const { return destRect; }
    bool checkCollision(const GameObject &other) const;

    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setPosition(int x, int y) { xpos = x; ypos = y; }

private:
    int xpos, ypos;
    int prevX, prevY;
    SDL_Texture *objtexture;
    SDL_Rect srcRect, destRect;

    int totalFrames;    // Tổng số frame cho animation di chuyển
    int totalRows;      // Tổng số hàng của sprite sheet (cho animation)
    int currentFrame;   // Frame hiện tại của animation di chuyển
    int currentRow;     // Hàng hiện tại (ví dụ: hàng cho animation di chuyển)
    int animSpeed;      // Tốc độ chuyển frame cho di chuyển

    int totalIdleFrames; // Tổng số frame của animation idle
    int idleFrame;       // Frame hiện tại của animation idle
    int idleAnimSpeed;   // Tốc độ chuyển frame cho idle

    Uint32 lastFrameTime; // Thời điểm cập nhật frame di chuyển cuối cùng
    Uint32 lastIdleTime;  // Thời điểm cập nhật frame idle cuối cùng

    int frameWidth;     // Chiều rộng của mỗi frame
    int frameHeight;    // Chiều cao của mỗi frame

    bool isMoving;      // Cờ xác định đối tượng đang di chuyển hay không
};
