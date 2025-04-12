#include "GameObject.h"
#include "TextureManager.h"
#include "Game.hpp"
#include "Physics.hpp"
#include "Combat.hpp"
#include <SDL.h>
#include <iostream>
#include <cstring> // Required for strcmp
#include <cmath>   // For std::fabs, std::fmod
#include <algorithm> // For std::min/max
#include <ctime>   // Required for std::time for seeding rng

// Helper function to load texture and get frame info
// Returns true on success, false on failure
bool GameObject::loadAnimationData(const char* path, SDL_Texture*& texture, int& totalFrames, int& frameWidth, int& frameHeight) {
    texture = TextureManager::loadTexture(path);
    if (texture == nullptr) {
        std::cout << "Failed to load texture: " << path << std::endl;
        totalFrames = 0;
        frameWidth = 0;
        frameHeight = 0;
        return false;
    }

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    frameHeight = h; // Assume single row

    // Set frame counts for each animation
    if (strcmp(path, "assets/Run.png") == 0) {
        totalFrames = 8; // Running animation has 8 frames
        frameWidth = w / totalFrames;
    } else if (strcmp(path, "assets/Idle.png") == 0) {
        totalFrames = 11;
        frameWidth = w / totalFrames;
    } else if (strcmp(path, "assets/Jump.png") == 0) {
        totalFrames = 4;
        frameWidth = w / totalFrames;
    } else if (strcmp(path, "assets/Attack.png") == 0) {
        totalFrames = 6; // Attack animation has 6 frames
        frameWidth = w / totalFrames;
    } else if (strcmp(path, "assets/Parry.png") == 0) {
        totalFrames = 4; // Assuming 4 frames for Parry
        frameWidth = w / totalFrames;
    } else {
        totalFrames = 1;
        frameWidth = w;
        std::cout << "Warning: Unknown texture path \"" << path << "\", assuming 1 frame." << std::endl;
    }

    return true;
}

GameObject::GameObject(const char* idleTexturePath, const char* runTexturePath, const char* jumpTexturePath, const char* attackTexturePath,
                       int x, int y, float scale)
    : // Match declaration order in GameObject.h
      // Public members first
      prevX(x), prevY(y),
      velocityX(0.0f), velocityY(0.0f), onGround(false), facingRight(true),
      destRect(), // Initialize later based on texture
      currentFrame(0), lastFrameTime(SDL_GetTicks()), currentState(IDLE),
      isAttacking(false), attackStartTime(0), attackDuration(300), // Default attack duration
      isParrying(false), parryFrameIndex(0), parryStartTime(0), parryDuration(300), // Default parry duration
      rng(static_cast<unsigned int>(std::time(0))),
      // Private members next
      xpos(x), ypos(y),
      scale(scale),
      srcRect(), // Initialize later based on texture
      collider(), // Initialize later based on destRect
      colliderOffsetX(5), colliderOffsetY(5),
      idleTexture(nullptr), runTexture(nullptr), jumpTexture(nullptr), attackTexture(nullptr), parryTexture(nullptr), currentTexture(nullptr),
      idleTotalFrames(0), runTotalFrames(0), jumpTotalFrames(0), attackTotalFrames(0), parryTotalFrames(0), currentTotalFrames(0),
      idleFrameWidth(0), idleFrameHeight(0), runFrameWidth(0), runFrameHeight(0),
      jumpFrameWidth(0), jumpFrameHeight(0), attackFrameWidth(0), attackFrameHeight(0),
      parryFrameWidth(0), parryFrameHeight(0), currentFrameWidth(0), currentFrameHeight(0),
      idleAnimSpeed(150), runAnimSpeed(100), jumpAnimSpeed(100), attackAnimSpeed(50), parryAnimSpeed(80), currentAnimSpeed(150),
      completeRunAnimation(false)
{
    // Load textures using the class member function (which now uses static TM)
    bool idleLoaded = loadAnimationData(idleTexturePath, idleTexture, idleTotalFrames, idleFrameWidth, idleFrameHeight);
    bool runLoaded = loadAnimationData(runTexturePath, runTexture, runTotalFrames, runFrameWidth, runFrameHeight);
    bool jumpLoaded = loadAnimationData(jumpTexturePath, jumpTexture, jumpTotalFrames, jumpFrameWidth, jumpFrameHeight);
    bool attackLoaded = loadAnimationData(attackTexturePath, attackTexture, attackTotalFrames, attackFrameWidth, attackFrameHeight);
    bool parryLoaded = loadAnimationData("assets/Parry.png", parryTexture, parryTotalFrames, parryFrameWidth, parryFrameHeight);

    if (!idleLoaded && !runLoaded && !jumpLoaded && !attackLoaded) { // Check critical textures
        std::cerr << "Error: Failed to load critical animations for GameObject!" << std::endl;
        // Handle error: set default size or state
        destRect = {x, y, 32, 32}; // Example default size
        srcRect = {0, 0, 0, 0};
        collider = {x + colliderOffsetX, y + colliderOffsetY, 32 - 2 * colliderOffsetX, 32 - colliderOffsetY };
        return;
    }

    // Add check for parry texture
    if (!parryLoaded) {
        std::cerr << "Warning: Failed to load Parry animation for GameObject! Parry functionality might be disabled." << std::endl;
        // Optionally disable parry functionality if texture is missing
    }

    // Initialize to Idle state (or fallback)
    if (idleLoaded) {
        setAnimation(IDLE);
    } else if (runLoaded) {
        setAnimation(RUNNING);
    } else if (jumpLoaded) {
         setAnimation(JUMPING);
    } // Fallback handled by currentTexture check below

    // Initialize rects based on current animation (if loaded)
    if(currentTexture != nullptr) { 
        destRect = { x, y, static_cast<int>(currentFrameWidth * scale), static_cast<int>(currentFrameHeight * scale) };
        srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };
        // Initialize collider based on destRect and offsets
        collider = { x + colliderOffsetX, y + colliderOffsetY, destRect.w - 2 * colliderOffsetX, destRect.h - colliderOffsetY }; 
    } else {
        // This case should ideally not be reached if error handling above is good
        std::cerr << "Warning: No current texture set after loading attempts." << std::endl;
        destRect = {x, y, 32, 32}; // Example default size
        srcRect = {0, 0, 0, 0};
        collider = {x + colliderOffsetX, y + colliderOffsetY, 32 - 2 * colliderOffsetX, 32 - colliderOffsetY };
    }

    // Calculate attack duration (now that frames/speed are known)
    if(attackTotalFrames > 0 && attackAnimSpeed > 0) {
        attackDuration = attackTotalFrames * attackAnimSpeed;
    } else {
        attackDuration = 300; // Default duration
    }

     // Calculate parry duration (can be fixed or animation-based)
    if(parryTotalFrames > 0 && parryAnimSpeed > 0) {
        parryDuration = parryTotalFrames * parryAnimSpeed;
    } else {
        parryDuration = 400; // Default parry window duration
    }

    std::cout << "GameObject created." << std::endl;
}

GameObject::~GameObject() {
    if (idleTexture) SDL_DestroyTexture(idleTexture);
    if (runTexture) SDL_DestroyTexture(runTexture);
    if (jumpTexture) SDL_DestroyTexture(jumpTexture); 
    if (attackTexture) SDL_DestroyTexture(attackTexture); // Clean up attack texture
}

void GameObject::setAnimation(AnimationState newState) {
    // Prevent resetting if already in the same state, unless it's Parry (needs specific frame setup)
    if (currentState == newState && currentTexture != nullptr && newState != PARRYING) return;

    switch (newState) {
        case IDLE:
            if (!idleTexture) return;
            currentTexture = idleTexture;
            currentTotalFrames = idleTotalFrames;
            currentFrameWidth = idleFrameWidth;
            currentFrameHeight = idleFrameHeight;
            currentAnimSpeed = idleAnimSpeed;
            break;
        case RUNNING:
            if (!runTexture) return;
            currentTexture = runTexture;
            currentTotalFrames = runTotalFrames;
            currentFrameWidth = runFrameWidth;
            currentFrameHeight = runFrameHeight;
            currentAnimSpeed = runAnimSpeed;
            break;
        case JUMPING:
            if (!jumpTexture) return;
            currentTexture = jumpTexture;
            currentTotalFrames = jumpTotalFrames;
            currentFrameWidth = jumpFrameWidth;
            currentFrameHeight = jumpFrameHeight;
            currentAnimSpeed = jumpAnimSpeed;
            break;
        case ATTACKING:
            if (!attackTexture) return;
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames;
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            currentAnimSpeed = attackAnimSpeed;
            break;
        case PARRYING:
            if (!attackTexture) return; // Use attack texture for parry frames
            currentTexture = attackTexture;
            currentTotalFrames = attackTotalFrames; // Use total frames for consistency
            currentFrameWidth = attackFrameWidth;
            currentFrameHeight = attackFrameHeight;
            // Parry doesn't use animation speed, it holds a frame
            currentAnimSpeed = 0; 
            break;
    }

    currentState = newState;
    // Reset frame only if not starting a parry (parry sets its own frame index)
    if (newState != PARRYING) {
       currentFrame = 0; 
    }
    lastFrameTime = SDL_GetTicks(); 
    
    // Set specific frame for Parry immediately
    if (currentState == PARRYING) {
        currentFrame = parryFrameIndex; 
        srcRect.x = parryFrameIndex * currentFrameWidth;
        srcRect.y = 0;
        srcRect.w = currentFrameWidth;
        srcRect.h = currentFrameHeight;
    } else {
        srcRect = { 0, 0, currentFrameWidth, currentFrameHeight };
    }

    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
}

void GameObject::parry() {
    // Can only parry if not already attacking or parrying
    if (!isAttacking && !isParrying) { 
        isParrying = true;
        parryStartTime = SDL_GetTicks();
        
        // Select a random frame from the attack animation
        if (attackTotalFrames > 0) {
            std::uniform_int_distribution<int> dist(0, attackTotalFrames - 1);
            parryFrameIndex = dist(rng);
        } else {
            parryFrameIndex = 0; // Fallback if attack animation isn't loaded
        }

        setAnimation(PARRYING);
        std::cout << "Parry started. Frame: " << parryFrameIndex << ", Duration: " << parryDuration << "ms" << std::endl;
    }
}

void GameObject::attack() {
    // Can only attack if not already attacking or parrying
    if (!isAttacking && !isParrying) { 
        isAttacking = true;
        attackStartTime = SDL_GetTicks();
        attackDuration = static_cast<Uint32>(attackTotalFrames * attackAnimSpeed);
        setAnimation(ATTACKING);
        std::cout << "Attack started. Duration: " << attackDuration << "ms" << std::endl;
    }
}

void GameObject::move(int dx) {
    // Update horizontal velocity based on input direction
    // A simple implementation might just add a fixed amount, 
    // but a max speed cap is usually desirable.
    float moveSpeed = 4.0f; // Example speed, adjust as needed
    if (dx > 0) {
        velocityX = moveSpeed;
        facingRight = true;
        
        // If not running, attacking, or parrying, start run animation
        if (currentState != RUNNING && !isAttacking && !isParrying) {
            setAnimation(RUNNING);
            completeRunAnimation = true;
        }
    } else if (dx < 0) {
        velocityX = -moveSpeed;
        facingRight = false;
        
        // If not running, attacking, or parrying, start run animation
        if (currentState != RUNNING && !isAttacking && !isParrying) {
            setAnimation(RUNNING);
            completeRunAnimation = true;
        }
    } 
    // Note: We don't set velocityX to 0 when dx is 0 here.
    // Friction in Physics::applyFriction handles slowing down when there's no input.
    // We also don't change xpos directly. Physics::applyFriction will handle that.
}

void GameObject::jump() {
    Physics::applyJump(this); 
}

void GameObject::revertPosition() { 
    xpos = prevX; 
    ypos = prevY; 
    destRect.x = xpos;
    destRect.y = ypos;
}

void GameObject::update() {
    Uint32 currentTime = SDL_GetTicks();

    // Handle parry state completion
    if (isParrying) {
        if (static_cast<int>(currentTime - parryStartTime) >= static_cast<int>(parryDuration)) {
            isParrying = false;
            setAnimation(IDLE); // Revert to idle after parry finishes
            std::cout << "Parry finished." << std::endl;
        }
        // While parrying, we don't process other state changes or animation updates below
    }
    // Handle attack state completion (only if not parrying)
    else if (isAttacking) {
        if (static_cast<int>(currentTime - attackStartTime) >= attackDuration) {
            isAttacking = false;
            setAnimation(IDLE); // Revert to idle after attack finishes
            std::cout << "Attack finished." << std::endl;
        }
         // While attacking, we don't process other state changes below
    }
    // Normal animation state logic (only if not attacking or parrying)
    else { // !isAttacking && !isParrying
        if (!onGround) {
            if (currentState != JUMPING) {
                completeRunAnimation = false; 
                setAnimation(JUMPING);
            }
        } else if (std::fabs(velocityX) > 0.1f) {
            if (currentState != RUNNING) {
                setAnimation(RUNNING);
                completeRunAnimation = true;
            }
        } else {
            if (currentState == RUNNING && completeRunAnimation) {
                if (currentFrame == runTotalFrames - 1) {
                    completeRunAnimation = false;
                    setAnimation(IDLE);
                }
            } else if (currentState != IDLE && !completeRunAnimation) {
                setAnimation(IDLE);
            }
        }
    }

    // Advance animation frame (only if not parrying)
    if (currentState != PARRYING && currentTotalFrames > 1 && 
        static_cast<int>(currentTime - lastFrameTime) > currentAnimSpeed) {
        currentFrame = (currentFrame + 1) % currentTotalFrames;
        lastFrameTime = currentTime;
        
        // Update srcRect.x based on the new frame for non-parry states
        srcRect.x = currentFrame * currentFrameWidth;
    } else if (currentState == PARRYING) {
         // Ensure the parry frame remains displayed
         srcRect.x = parryFrameIndex * currentFrameWidth;
    }

    // Update remaining srcRect parts (always needed)
    srcRect.y = 0;
    srcRect.w = currentFrameWidth;
    srcRect.h = currentFrameHeight;
    
    destRect.w = static_cast<int>(currentFrameWidth * scale);
    destRect.h = static_cast<int>(currentFrameHeight * scale);
}

void GameObject::render() {
    if (!Game::renderer || !currentTexture) return;
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &destRect, 0, NULL, flip);
}

void GameObject::render(int x, int y) {
    if (!Game::renderer || !currentTexture) return;
    SDL_Rect tempDestRect = { x, y, destRect.w, destRect.h };
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(Game::renderer, currentTexture, &srcRect, &tempDestRect, 0, NULL, flip);
}

SDL_Rect GameObject::getAttackHitbox() const {
    // Only return a valid hitbox if the player is attacking
    if (currentState != ATTACKING) {
        // Return empty hitbox if not attacking
        return {0, 0, 0, 0};
    }
    
    // Create a hitbox that extends in front of the player in the direction they're facing
    SDL_Rect attackBox;
    int hitboxSize = destRect.w / 2; // Half the width of player
    
    // Position the hitbox based on facing direction - make it closer to the player
    if (facingRight) {
        // Place hitbox to the right of player, but closer (1/4 width extension instead of 1/2)
        attackBox.x = destRect.x + destRect.w/4;
    } else {
        // Place hitbox to the left of player, but closer (1/4 width extension)
        attackBox.x = destRect.x - hitboxSize/2;
    }
    
    // Vertical position (centered on player's upper body)
    attackBox.y = destRect.y + destRect.h/4;
    attackBox.w = hitboxSize;
    attackBox.h = destRect.h/2;
    
    return attackBox;
}
