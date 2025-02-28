#include "textureManager.h"
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
