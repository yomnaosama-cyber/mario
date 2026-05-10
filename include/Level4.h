#ifndef LEVEL4_H
#define LEVEL4_H

#include "Level.h"

class Level4 : public Level {
public:
    Level4(int rows, int cols);
    void createTiles() override;
};

#endif