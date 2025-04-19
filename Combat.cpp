#include "Combat.hpp"
#include "GameObject.h"
#include <SDL.h>

namespace Combat {
    void attack(GameObject* player) {
        // Only set attack state if player isn't already attacking
        if (!player->isAttacking) {
            player->isAttacking = true;
            player->attackStartTime = SDL_GetTicks();
            player->attackDuration = 300; // Attack animation duration in milliseconds
        }
    }

    void parry() {
        // Parry functionality is implemented directly in GameObject class
    }
}
