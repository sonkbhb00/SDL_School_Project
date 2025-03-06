#include "GameObject.h"
#include "textureManeger.h"
#include <iostream>
using namespace std;

GameObject::GameObject(const char* texture, int x, int y, int totalFrames, int totalRows, int animSpeed) {
    objtexture = textureManeger::loadTexture(texture);
    if(objtexture == nullptr){
        cout << "Texture not loaded!" << endl;
    }
    xpos = x;
    ypos = y;
    prevX = x;
    prevY = y;

    // Lưu các thông số animation
    this->totalFrames = totalFrames;
    this->totalRows = totalRows;
    this->animSpeed = animSpeed;
    currentFrame = 0;
    currentRow = 0;

    // Lấy kích thước của toàn bộ sprite sheet
    int textureWidth, textureHeight;
    SDL_QueryTexture(objtexture, NULL, NULL, &textureWidth, &textureHeight);

    // Tính kích thước của mỗi frame
    frameWidth = textureWidth / totalFrames;
    frameHeight = textureHeight / totalRows;

    // Khởi tạo srcRect và destRect
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    destRect.x = xpos;
    destRect.y = ypos;
    destRect.w = frameWidth;
    destRect.h = frameHeight;

    isMoving = false; // Ban đầu player không di chuyển
}


void GameObject::Render() {
    // Nếu có animation (nhiều frame) thì vẽ frame cụ thể, ngược lại vẽ toàn bộ texture
    if(totalFrames > 1 || totalRows > 1)
        SDL_RenderCopy(Game::renderer, objtexture, &srcRect, &destRect);
    else
        SDL_RenderCopy(Game::renderer, objtexture, NULL, &destRect);
}

void GameObject::Move(int dx, int dy) {
    // Lưu vị trí cũ trước khi di chuyển
    prevX = xpos;
    prevY = ypos;

    // Cập nhật vị trí mới
    xpos += dx;
    ypos += dy;

    // Đánh dấu rằng có di chuyển
    isMoving = true;
}

void GameObject::revertPosition() {
    xpos = prevX;
    ypos = prevY;
}

bool GameObject::checkCollision(const GameObject &other) const {
    SDL_Rect a = destRect;
    SDL_Rect b = other.destRect;
    if(a.x + a.w <= b.x) return false;
    if(a.x >= b.x + b.w) return false;
    if(a.y + a.h <= b.y) return false;
    if(a.y >= b.y + b.h) return false;
    return true;
}
// Thêm biến thành viên: Uint32 lastFrameTime = 0; (khởi tạo trong constructor)
void GameObject::Update() {
    // Cập nhật vị trí cho destination rectangle
    destRect.x = xpos;
    destRect.y = ypos;

    // Nếu có di chuyển, cập nhật frame animation
    if(isMoving) {
        currentFrame = (currentFrame + 1) % totalFrames;
        srcRect.x = currentFrame * frameWidth;
        // Nếu bạn có nhiều hàng (ví dụ: cho các trạng thái khác nhau), cập nhật srcRect.y tương ứng với currentRow
        // srcRect.y = currentRow * frameHeight;
    }

    // Nếu không di chuyển, bạn có thể đặt lại frame về trạng thái idle (ví dụ: frame 0)
    // else {
    //     currentFrame = 0;
    //     srcRect.x = 0;
    // }

    // Reset cờ di chuyển cho lần cập nhật sau
    isMoving = false;
}
