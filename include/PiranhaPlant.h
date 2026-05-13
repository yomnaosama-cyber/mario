#ifndef PIRANHA_PLANT_H
#define PIRANHA_PLANT_H

#include "Enemy.h"

class PiranhaPlant : public Enemy {
private:
    int baseY;
    bool goingUp;
    int pixelX;
    int pixelY;

public:
    PiranhaPlant(int x, int y);
    void update();
    void setPixelPos(int x, int y);
    int getPixelX() const;
    int getPixelY() const;

   
};

#endif