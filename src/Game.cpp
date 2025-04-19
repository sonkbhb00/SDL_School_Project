// Apply physics to enemy
Physics::applyGravity(*enemy);
Physics::applyFriction(*enemy);
Physics::checkCollisionSide(*enemy, map);
Physics::isOnGround(*enemy, map);

// Simple AI: Move towards a point (e.g., x=400) then stop
if (std::abs(enemy->getXpos() - 400) > 5) {
    enemy->aiMoveTowards(400);
} else {
    enemy->setState(EnemyState::ENEMY_IDLE);
}

// Update enemy state and animation
enemy->update();

// Player attack hit detection
if (player->getState() == State::ATTACKING) {
    SDL_Rect playerAttackBox = player->getAttackHitbox(); // Assuming you have a function to get the attack hitbox
    SDL_Rect enemyCollider = enemy->getCollider();

    // Offset hitboxes by camera position if camera is not locked
    if (!lockCamera) {
        playerAttackBox.x -= camera.x;
        playerAttackBox.y -= camera.y;
        enemyCollider.x -= camera.x;
        enemyCollider.y -= camera.y;
    }

    if (SDL_HasIntersection(&playerAttackBox, &enemyCollider) && !enemy->isTakingHit()) {
        enemy->takeHit();
    }
}

// Update camera position if not locked 

// Initialize Enemy
// Corrected constructor call with Take Hit texture path
enemy = new Enemy("assets/Idle.png", "assets/Run.png", "assets/Take Hit.png", 250, 50, 1.0f);
if (!enemy) {
    std::cerr << "Failed to create enemy object!" << std::endl;
}


void Game::Game() {
    isRunning = false;
    isPaused = false;
    showInstructions = false;
}

// In the init method, load the instruction image
void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    // Load instruction image
    instructionTexture = new TextureManager();
    instructionTexture->LoadTexture("C:\\Users\\Bui Son\\OneDrive\\Desktop\\demo_sdl\\demo\\assets\\instruction.png");
}

// In the handleEvents method, add F1 key detection
void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;
                case SDLK_SPACE:
                    isPaused = !isPaused;
                    break;
                case SDLK_F1:
                    showInstructions = !showInstructions;
                    break;
            }
            break;
    }
}

// In the render method, display the instruction image when F1 is pressed
void Game::render() {
    SDL_RenderClear(renderer);
    
    // Render game objects
    map->DrawMap();
    player->Render();
    enemy->Render();
    
    // Render instructions if F1 is pressed (on top of everything)
    if (showInstructions) {
        SDL_Rect instructionRect = { 
            0,  // Set x to 0 to start from left edge
            0,  // Set y to 0 to start from top edge
            SCREEN_WIDTH,  // Use full screen width
            SCREEN_HEIGHT  // Use full screen height
        };
        SDL_RenderCopy(renderer, instructionTexture->getTexture(), NULL, &instructionRect);
    }
    
    SDL_RenderPresent(renderer);
}

// In the clean method, clean up the instruction texture
void Game::clean() {
    if (instructionTexture) {
        delete instructionTexture;
        instructionTexture = nullptr;
    }
}