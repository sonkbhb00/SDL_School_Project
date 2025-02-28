#pragma once
#include "game.hpp"
class GameObject{
public:
    GameObject(const char* texture, int x, int y);
    ~GameObject();
    void Update();
    void Render();

private:
    int xpos,ypos;
    SDL_Texture *objtexture;

    SDL_Rect srcRect, destRect;
};
