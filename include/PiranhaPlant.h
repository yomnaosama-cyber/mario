#ifndef PIRANHA_PLANT_H
#define PIRANHA_PLANT_H
#include "Enemy.h"

class PiranhaPlant : public Enemy {
private:
    float bobOffset = 0.f;
    float bobVelocity = -1.5f;

public:
    PiranhaPlant(int x_pos, int y_pos);

    void update();
    float getBobPixelOffset() const { return bobOffset; }
};

#endif
