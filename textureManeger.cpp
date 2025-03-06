#include "textureManeger.h"


SDL_Texture * textureManeger::loadTexture(const char* texture){
    SDL_Surface *tmpSurface = IMG_Load(texture);
    if(tmpSurface == nullptr){
        cout << "Failed to load surface: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(Game::renderer, tmpSurface);
    if(tex == nullptr){
        cout << "Failed to create texture: " << SDL_GetError() << endl;
    }
    SDL_FreeSurface(tmpSurface);
    return tex;
}
void textureManeger::draw(SDL_Texture *texture, SDL_Rect srcR, SDL_Rect destR) {
    SDL_RenderCopy(Game::renderer, texture, &srcR, &destR);
}
void textureManeger::drawFrame(SDL_Texture* texture, int frame, int row,
                               int frameWidth, int frameHeight,
                               int x, int y)
{
    // Tính toán vùng cắt (source rectangle)
    SDL_Rect srcRect;
    srcRect.x = frame * frameWidth;  // frame xác định ô theo chiều ngang
    srcRect.y = row * frameHeight;   // row xác định hàng của sprite
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    // Tạo vùng đích (destination rectangle)
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = frameWidth;   // có thể scale nếu muốn
    destRect.h = frameHeight;

    // Vẽ frame lên renderer
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}
