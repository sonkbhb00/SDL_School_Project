#ifndef COMBAT_HPP
#define COMBAT_HPP

// Forward declaration của GameObject
class GameObject;

namespace Combat {
    // Hàm attack nhận con trỏ player và chỉ set trạng thái attack
    void attack(GameObject* player);
    void parry();
    void shoot();
    void poison();
    void reload();
}

#endif // COMBAT_HPP
