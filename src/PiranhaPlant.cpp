#include "PiranhaPlant.h"

PiranhaPlant::PiranhaPlant(int x, int y) : Enemy(x, y) {}

void PiranhaPlant::update() {
    // simple up/down movement
    if (getY() % 2 == 0)
        setY(getY() + 1);
    else
        setY(getY() - 1);
}