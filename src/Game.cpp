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