#include "Enemy.h"

Enemy::Enemy(int x, int y)
    : Entity(x, y, 50, "Enemy"), direction(1), speed(0.5f), minX(0), maxX(79), preciseX(static_cast<float>(x)) {
    startX = x;
}

Enemy::Enemy(int x, int y, int minX, int maxX, float speed)
    : Entity(x, y, 50, "Enemy"), direction(1), speed(speed), minX(minX), maxX(maxX), preciseX(static_cast<float>(x)) {
    startX = x;
}

void Enemy::autoMove() {
    // Use a float position so speeds below 1 tile/frame still move.
    preciseX += direction * speed;
    
    // Keep enemy within its boundaries
    if (preciseX <= minX) {
        preciseX = static_cast<float>(minX);
        direction = 1;   // Move right
    }
    else if (preciseX >= maxX) {
        preciseX = static_cast<float>(maxX);
        direction = -1;  // Move left
    }

    x = static_cast<int>(preciseX);
}