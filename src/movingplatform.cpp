#include "movingplatform.h"

MovingPlatform::MovingPlatform(long long x, long long y,int speed, int min, int max, Axis direc): Tile(x,y, Tile::Brick), speed(speed), min(min), max(max), direc(direc){}

void MovingPlatform::update(){

    if(direc == Horizontal){
        x+=speed;
        if(x == max || x == min){
            speed = -speed;
        }
    }
    else{
        y+=speed;
        if (y == max || y == min){
            speed = -speed;
        }
    }
};
