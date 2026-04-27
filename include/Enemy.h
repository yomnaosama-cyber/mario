#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

class Enemy : public Entity {
private:
    int direction;      // -1 = left, +1 = right
    float speed;        // Movement speed (lower = slower)
    int minX;           // Left boundary (tile coordinates)
    int maxX;           // Right boundary (tile coordinates)
    int startX;         // Starting X position
    float preciseX;     // Sub-tile X position for smooth movement

public:
    Enemy(int x, int y);
    Enemy(int x, int y, int minX, int maxX, float speed);  // NEW constructor

    void autoMove();
    float getPreciseX() const { return preciseX; }
    void setBoundaries(int min, int max) { minX = min; maxX = max; }
    void setSpeed(float s) { speed = s; }
};

#endif