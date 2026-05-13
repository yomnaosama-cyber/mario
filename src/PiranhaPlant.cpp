#include "PiranhaPlant.h"

PiranhaPlant::PiranhaPlant(int x_pos, int y_pos)
    : Enemy(x_pos, y_pos, x_pos, x_pos, 0.f) {}

void PiranhaPlant::update() {
    bobOffset += bobVelocity;
    if (bobOffset < -55.f) {
        bobOffset = -55.f;
        bobVelocity = 1.2f;
    } else if (bobOffset > 5.f) {
        bobOffset = 5.f;
        bobVelocity = -1.2f;
    }
}

/*
#include "PiranhaPlant.h"
#include "Enemy.h"

PiranhaPlant::PiranhaPlant(int x, int y) : Enemy(x, y) {}

void PiranhaPlant::update() {
    // simple up/down movement
    if (getY() % 2 == 0)
        setY(getY() + 1);
    else
        setY(getY() - 1);
}
*/
