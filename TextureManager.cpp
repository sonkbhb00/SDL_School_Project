#include "TextureManager.h"
#include "Game.hpp"
#include <SDL_image.h>
#include <iostream>
#include <vector>

// Assuming Game.hpp is included via TextureManager.h for Game::renderer

// Keep track of textures to clean up
namespace {
    std::vector<SDL_Texture*> loadedTextures;
}

SDL_Texture* TextureManager::loadTexture(const char* filePath) {
    SDL_Surface* tempSurface = IMG_Load(filePath);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << filePath << ". Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!texture) {
        std::cout << "Failed to create texture from surface. Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    loadedTextures.push_back(texture);
    return texture;
}

// Revert to using Game::renderer
void TextureManager::draw(SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect) {
    if (!Game::renderer || !texture) return;
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}

// Revert to using Game::renderer
void TextureManager::drawFrame(SDL_Texture* texture, int frame, int row, int frameWidth, int frameHeight, int x, int y) {
    if (!Game::renderer || !texture) return;
    SDL_Rect srcRect = { frameWidth * frame, frameHeight * row, frameWidth, frameHeight };
    SDL_Rect destRect = { x, y, frameWidth, frameHeight };
    SDL_RenderCopy(Game::renderer, texture, &srcRect, &destRect);
}

void TextureManager::cleanUp() {
    for (SDL_Texture* texture : loadedTextures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    loadedTextures.clear();
    std::cout << "TextureManager cleaned up textures." << std::endl;
}

