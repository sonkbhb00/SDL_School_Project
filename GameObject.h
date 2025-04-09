#pragma once
#include "Game.hpp"
#include <SDL.h>

// Forward declarations
class Physics;
namespace Combat {
    void attack(class GameObject* player);
    void parry();
    void shoot();
    void poison();
    void reload();
}

class GameObject {
public:
    // Constructor nhận texturePath, vị trí, scale, tổng frame, số hàng và tốc độ animation ban đầu
    GameObject(const char* texturePath, int x, int y, float scale = 1.0f,
               int totalFrames = 1, int totalRows = 1, int animSpeed = 100);
    ~GameObject();

    void update();
    void render();
    void move(int dx, int dy);
    void revertPosition();

    // Trả về collider dùng cho xử lý va chạm
    SDL_Rect getCollider() const { return destRect; }
    bool checkCollision(const GameObject &other) const;

    // Getters và setters vị trí
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setPosition(int x, int y) { xpos = x; ypos = y; }

    // Getters và setters cho texture và thông số animation
    SDL_Texture* getTexture() const;
    void setTexture(SDL_Texture* newTexture);

    int getTotalFrames() const;
    void setTotalFrames(int frames);
    int getTotalRows() const;
    void setTotalRows(int rows);
    int getAnimSpeed() const;
    void setAnimSpeed(int speed);

    void resetAnimation();

    // Nếu cần, thêm setter cho isMoving (không bắt buộc)
    void setIsMoving(bool moving) { isMoving = moving; }

    // Các biến trạng thái cho attack (sử dụng khi click chuột, …)
    bool isAttacking;
    Uint32 attackStartTime;
    int attackDuration;
    SDL_Texture* originalTexture;
    int originalTotalFrames;
    int originalTotalRows;
    int originalAnimSpeed;

    // Thông số animation cho trạng thái idle
    int idleTotalFrames;  // Ví dụ: 4 frame idle
    int idleAnimSpeed;    // Ví dụ: 300ms giữa các frame idle
    int idleRow;          // Hàng dùng cho idle animation

    // Thông số animation cho trạng thái chạy
    int runTotalFrames;   // Ví dụ: 6 frame chạy
    int runAnimSpeed;     // Ví dụ: 100ms giữa các frame chạy
    int runRow;           // Hàng dùng cho chạy animation

private:
    int xpos, ypos;
    int prevX, prevY;
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;

    // Các thông số animation hiện hành
    int totalFrames;
    int totalRows;
    int currentFrame;
    int currentRow; // Hàng hiện hành của sprite sheet
    int animSpeed;
    Uint32 lastFrameTime;
    int frameWidth;
    int frameHeight;
    bool isMoving;
    float scale;
    float velocityY;
    bool facingRight;

    friend class Physics;
    friend void Combat::attack(GameObject* player);
    friend void Combat::parry();
    friend void Combat::shoot();
    friend void Combat::poison();
    friend void Combat::reload();
};
