#pragma once
#include "Game.hpp"
#include <SDL.h>
#include <vector>

// Forward declare Physics class
class Physics;

// Forward declarations
namespace Combat {
    void attack(class GameObject* player);
    void parry();
    void shoot();
    void poison();
    void reload();
}

// Enum for animation states
enum AnimationState {
    IDLE,
    RUNNING,
    JUMPING
    // FALLING could be another state
};

class GameObject {
public:
    // Removed Physics Constants

    // Constructor now takes paths for idle, run, and jump textures
    GameObject(const char* idleTexturePath, const char* runTexturePath, const char* jumpTexturePath,
               int x, int y, float scale = 1.0f,
               int idleAnimSpeed = 150, int runAnimSpeed = 100, int jumpAnimSpeed = 100);
    ~GameObject();

    void update(); // Handles animation state logic
    void render(); // Keep original signature for backwards compatibility
    void render(int x, int y); // Add overload that takes position
    
    void move(int dx); // Updated signature
    void jump(); // Kept jump action method
    void revertPosition();

    SDL_Rect getCollider() const { return destRect; }
    bool checkCollision(const GameObject &other) const;

    // Position Getters/Setters
    int getX() const { return xpos; }
    int getY() const { return ypos; }
    void setPosition(int x, int y) { xpos = x; ypos = y; }
    void setX(int x) { xpos = x; }
    void setY(int y) { ypos = y; }

    // Texture Getters/Setters (Consider removing if textures are managed internally)
    // SDL_Texture* getTexture() const { return currentTexture; } // currentTexture is now internal
    // void setTexture(SDL_Texture* newTexture);

    // Public state variables (Consider making private)
    int currentFrame;
    Uint32 lastFrameTime;
    AnimationState currentState;

    // Attack state variables
    bool isAttacking;
    Uint32 attackStartTime;
    int attackDuration;

    // Physics properties
    bool onGround;
    float velocityX;
    float velocityY;

    bool facingRight;
    
    // Add members needed by Physics class directly (make public or use friends)
    // These were likely accessed directly or via getters by the old Physics class
    // Need to ensure Physics.cpp can access necessary GameObject members.
    // For simplicity, keep them public for now or re-add friend declaration.
    int xpos, ypos; // Make position public temporarily if needed by Physics
    SDL_Rect destRect; // Physics might need this
    int prevX, prevY; // Physics might need this

private:
    // int xpos, ypos; // Moved to public for Physics access
    // int prevX, prevY; // Moved to public
    SDL_Rect srcRect;
    // SDL_Rect destRect; // Moved to public
    float scale;

    // Animation Data
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* jumpTexture;
    SDL_Texture* currentTexture; // Texture currently being used for rendering

    int idleTotalFrames;
    int runTotalFrames;
    int jumpTotalFrames;
    int currentTotalFrames; // Frame count for the current animation

    int idleFrameWidth, idleFrameHeight;
    int runFrameWidth, runFrameHeight;
    int jumpFrameWidth, jumpFrameHeight;
    int currentFrameWidth, currentFrameHeight; // Dimensions for the current animation

    int idleAnimSpeed;
    int runAnimSpeed;
    int jumpAnimSpeed;
    int currentAnimSpeed; // Speed for the current animation

    // Private helper to switch animations
    void setAnimation(AnimationState newState);

    // Friend declarations
    friend class Physics; // Re-add friend declaration
    friend void Combat::attack(GameObject* player);
};
