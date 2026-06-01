#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Tile.h"
class MovingPlatform : public Tile
{
public:
    enum Axis {Horizontal, Vertical};

private:
    int speed;
    int min;
    int max;
    Axis direc;
    int deltaX = 0;
    int deltaY = 0;


public:

    MovingPlatform(long long x, long long y,int speed, int min, int max, Axis direc);

    void update();
    int getDeltaX() const {return deltaX;}
    int getDeltaY() const {return deltaY;}

};

#endif // MOVINGPLATFORM_H
