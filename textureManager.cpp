#include "textureManager.h"
SDL_Texture * textureManeger::loadTexture(const char* texture, SDL_Renderer *render){
    SDL_Surface *tmpSurface = IMG_Load(texture);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(render, tmpSurface);
    SDL_FreeSurface(tmpSurface);

    return tex;

}
