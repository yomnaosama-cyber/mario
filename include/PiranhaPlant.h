#ifndef PIRANHA_PLANT_H
#define PIRANHA_PLANT_H

#include "Entity.h"

class PiranhaPlant : public Entity {
private:
    int baseY;
    bool goingUp;

public:
    PiranhaPlant(int x, int y);
        : Enemy(x, y, x, x, 0), baseY(y), goingUp(true) {}

    void update() {
        // simple up/down movement
        if (goingUp) {
            setY(getY() - 1);
            if (getY() < baseY - 2)
                goingUp = false;
        } else {
            setY(getY() + 1);
            if (getY() > baseY)
                goingUp = true;
        }
    }
};

#endif