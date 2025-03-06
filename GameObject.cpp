#include "GameObject.h"
#include "TextureManager.h"
#include <SDL.h>
#include <iostream>

using namespace std;

GameObject::GameObject(const char* texturePath, int x, int y, int totalFrames, int totalRows, int animSpeed)
    : xpos(x), ypos(y), prevX(x), prevY(y), totalFrames(totalFrames), totalRows(totalRows),
      animSpeed(animSpeed), currentFrame(0), currentRow(0), lastFrameTime(0), isMoving(false)
{
    texture = TextureManager::loadTexture(texturePath);
    if (texture == nullptr) {
        cout << "Texture not loaded!" << endl;
    }
    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
    frameWidth = textureWidth / totalFrames;
    frameHeight = textureHeight / totalRows;

    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { xpos, ypos, frameWidth, frameHeight };
}

GameObject::~GameObject() {
    // Không xóa texture tại đây vì TextureManager quản lý nó
}

void GameObject::update() {
    destRect.x = xpos;
    destRect.y = ypos;
    if (isMoving) {
        currentFrame = (currentFrame + 1) % totalFrames;
        srcRect.x = currentFrame * frameWidth;
    }
    isMoving = false;
}

void GameObject::render() {
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}

void GameObject::move(int dx, int dy) {
    prevX = xpos;
    prevY = ypos;
    xpos += dx;
    ypos += dy;
    isMoving = true;
}

void GameObject::revertPosition() {
    xpos = prevX;
    ypos = prevY;
}
