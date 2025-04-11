#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>

SDL_Texture* TextureManager::loadTexture(const char* filePath) {
    SDL_Surface* tmpSurface = IMG_Load(filePath);
    if (tmpSurface == nullptr) {
        std::cout << "Failed to load surface: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, tmpSurface);
    if (texture == nullptr) {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(tmpSurface);
    return texture;
}

void TextureManager::draw(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect) {
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}

void TextureManager::drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y) {
    SDL_Rect srcRect = { frame * frameWidth, row * frameHeight, frameWidth, frameHeight };
    SDL_Rect destRect = { x, y, frameWidth, frameHeight };
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}

