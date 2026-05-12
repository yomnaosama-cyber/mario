#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "Tile.h"
class MovingPlatform : public Tile
{
public:
    enum Axis {Horizontal, Vetical};

private:
    int speed;
    int min;
    int max;
    Axis direc;

public:

    MovingPlatform(long long x, long long y,int speed, int min, int max, Axis direc);

    void update();


};

#endif // MOVINGPLATFORM_H
