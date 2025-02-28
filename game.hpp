#pragma once
#include <SDL_image.h>
#include <iostream>
using namespace std;
class Game{
public:
    Game();
    ~Game();

    void init(const char *title, int xPos, int yPos, int width, int height);

    void handleEvents();
    void update();
    void render();
    void clean();
    bool running(){
    return isRunning;};

    static SDL_Renderer* renderer;


private:
    bool isRunning;
    int cnt = 0;
    SDL_Window *window;

};

