#include "PiranhaPlant.h"

PiranhaPlant::PiranhaPlant(int x, int y)
    : Enemy(x, y), baseY(y), goingUp(true) {}

void PiranhaPlant::update() {
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