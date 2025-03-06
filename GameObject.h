#pragma once
#include "Game.hpp"

class GameObject {
public:
    GameObject(const char* texturePath, int x, int y, int totalFrames = 1, int totalRows = 1, int animSpeed = 100);
    ~GameObject();

    void update();
    void render();
    void move(int dx, int dy);
    void revertPosition();

    SDL_Rect getCollider() const { return destRect; }
    bool checkCollision(const GameObject &other) const;

    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setPosition(int x, int y) { xpos = x; ypos = y; }

private:
    int xpos, ypos;
    int prevX, prevY;
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;

    int totalFrames;
    int totalRows;
    int currentFrame;
    int currentRow;
    int animSpeed;

    Uint32 lastFrameTime; // Có thể dùng cho animation sau này

    int frameWidth;
    int frameHeight;

    bool isMoving;
};
