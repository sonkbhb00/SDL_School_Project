#include "GameObject.h"
#include "TextureManager.h"
#include "Game.hpp"
#include <SDL.h>

GameObject::GameObject(const char* texturePath, int x, int y, float scale, int totalFrames, int totalRows, int animSpeed)
    : xpos(x), ypos(y), scale(scale), totalFrames(totalFrames), totalRows(totalRows), animSpeed(animSpeed),
      currentFrame(0), currentRow(0), lastFrameTime(0), isMoving(false), velocityY(0), facingRight(true),
      isAttacking(false)
{
    texture = TextureManager::loadTexture(texturePath);
    if (texture == nullptr) {
        // Xử lý lỗi nếu texture không tải được
    }
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameWidth = w / totalFrames;
    frameHeight = h / totalRows;
    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { x, y, static_cast<int>(frameWidth * scale), static_cast<int>(frameHeight * scale) };

    // Khởi tạo thông số cho idle animation (giả sử nằm ở row 0)
    idleTotalFrames = totalFrames;
    idleAnimSpeed = animSpeed;
    idleRow = 0;

    // Khởi tạo thông số cho running animation (giả sử nằm ở row 1)
    runTotalFrames = totalFrames; // Có thể thay đổi nếu sprite sheet khác
    runAnimSpeed = animSpeed;       // Có thể hiệu chỉnh tốc độ cho hợp lý
    runRow = 1;
}

GameObject::~GameObject() {
    // Nếu không quản lý texture trong TextureManager, có thể giải phóng ở đây:
    // SDL_DestroyTexture(texture);
}

void GameObject::move(int dx, int dy) {
    prevX = xpos;
    prevY = ypos;
    xpos += dx;
    ypos += dy;
    isMoving = true; // Đánh dấu đối tượng đang di chuyển
}

void GameObject::revertPosition() {
    xpos = prevX;
    ypos = prevY;
}

SDL_Texture* GameObject::getTexture() const {
    return texture;
}

void GameObject::setTexture(SDL_Texture* newTexture) {
    texture = newTexture;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameWidth = w / totalFrames;
    frameHeight = h / totalRows;
    srcRect = { 0, 0, frameWidth, frameHeight };
}

int GameObject::getTotalFrames() const {
    return totalFrames;
}

void GameObject::setTotalFrames(int frames) {
    totalFrames = frames;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameWidth = w / totalFrames;
}

int GameObject::getTotalRows() const {
    return totalRows;
}

void GameObject::setTotalRows(int rows) {
    totalRows = rows;
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameHeight = h / totalRows;
}

int GameObject::getAnimSpeed() const {
    return animSpeed;
}

void GameObject::setAnimSpeed(int speed) {
    animSpeed = speed;
}

void GameObject::resetAnimation() {
    currentFrame = 0;
    // Chọn row animation dựa trên trạng thái di chuyển
    currentRow = isMoving ? runRow : idleRow;
    lastFrameTime = SDL_GetTicks();
}

void GameObject::update() {
    Uint32 currentTime = SDL_GetTicks();

    // Nếu đang tấn công, cập nhật animation tấn công
    if (isAttacking) {
        if (currentTime - lastFrameTime >= static_cast<Uint32>(animSpeed)) {
            currentFrame = (currentFrame + 1) % totalFrames;
            lastFrameTime = currentTime;
        }
    }
    // Nếu không tấn công, kiểm tra trạng thái di chuyển
    else if (isMoving) {
        // Nếu đang di chuyển, cập nhật running animation
        currentRow = runRow;
        if (currentTime - lastFrameTime >= static_cast<Uint32>(runAnimSpeed)) {
            currentFrame = (currentFrame + 1) % runTotalFrames;
            lastFrameTime = currentTime;
        }
    } else {
        // Nếu không di chuyển, cập nhật idle animation
        currentRow = idleRow;
        if (currentTime - lastFrameTime >= static_cast<Uint32>(idleAnimSpeed)) {
            currentFrame = (currentFrame + 1) % idleTotalFrames;
            lastFrameTime = currentTime;
        }
    }

    // Cập nhật vị trí trong sprite sheet
    srcRect.x = currentFrame * frameWidth;
    srcRect.y = currentRow * frameHeight;

    // Cập nhật vị trí render
    destRect.x = xpos;
    destRect.y = ypos;
    destRect.w = static_cast<int>(frameWidth * scale);
    destRect.h = static_cast<int>(frameHeight * scale);

    // Reset cờ isMoving
    isMoving = false;
}
void GameObject::render() {
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}
