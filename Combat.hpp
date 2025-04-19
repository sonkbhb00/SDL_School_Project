#ifndef COMBAT_HPP
#define COMBAT_HPP

// Forward declaration of GameObject
class GameObject;

namespace Combat {
    // Sets the attack state for the player
    void attack(GameObject* player);
    
    // Sets the parry state
    void parry();
}

#endif // COMBAT_HPP
