#include "GameObject.h"
#include "textureManager.h"
GameObject::GameObject(const char* texture , int x, int y){
    objtexture = textureManeger::loadTexture(texture);
    if(objtexture == nullptr){
        cout << "Texture not loaded!" << endl;
    }
    xpos = x;
    ypos = y;
}

void GameObject::Update(){
    int w, h;
    SDL_QueryTexture(objtexture, NULL, NULL, &w, &h);
    xpos++;
    ypos++;

    srcRect.h = h;
    srcRect.w = w;
    srcRect.x = 0;
    srcRect.y = 0;

    destRect.h = h/3;
    destRect.w = w/3;
    destRect.x = xpos;
    destRect.y = ypos;

    cout << "GameObject pos: (" << xpos << ", " << ypos << ")" << endl;
}

void GameObject::Render(){
    SDL_RenderCopy(Game::renderer, objtexture, &srcRect, &destRect);
}
