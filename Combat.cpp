#include "Combat.hpp"
#include "GameObject.h"
#include "TextureManager.h"
#include <SDL.h>
#include <iostream>

namespace Combat {

    void attack(GameObject* player) {
        // Nếu player chưa ở trạng thái attack
        if (!player->isAttacking) {
            // Lưu lại trạng thái ban đầu của player
            player->originalTexture = player->getTexture();
            player->originalTotalFrames = player->getTotalFrames();
            player->originalTotalRows = player->getTotalRows();
            player->originalAnimSpeed = player->getAnimSpeed();

            SDL_Texture* attackTexture = TextureManager::loadTexture("assets\\Attack.png");
            if (!attackTexture) {
                std::cout << "Failed to load attack texture!" << std::endl;
                return;
            }

            // Cập nhật texture và thông số cho attack animation
            player->setTexture(attackTexture);
            player->setTotalFrames(6);
            player->setTotalRows(1);
            player->setAnimSpeed(50);   // Thời gian giữa các frame (ms)
            player->resetAnimation();

            // Đánh dấu player đang attack
            player->isAttacking = true;
            player->attackStartTime = SDL_GetTicks();
            player->attackDuration = 300;

            std::cout << "Player attack animation started." << std::endl;
        }
    }

    void parry() {
        std::cout << "Player parries!" << std::endl;
    }

    void shoot() {
        std::cout << "Player shoots!" << std::endl;
    }

    void poison() {
        std::cout << "Player uses poison!" << std::endl;
    }

    void reload() {
        std::cout << "Player reloads!" << std::endl;
    }
}
