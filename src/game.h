// ... existing code ...

class Game {
public:
    Game();
    ~Game();
    
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    
    void handleEvents();
    void update();
    void render();
    void clean();
    
    bool running() { return isRunning; }
    
    static SDL_Renderer *renderer;
    static SDL_Event event;
    
private:
    bool isRunning;
    bool isPaused;
    bool showInstructions;
    SDL_Window *window;
    TextureManager* instructionTexture;
    
    // ... other private members ...
};