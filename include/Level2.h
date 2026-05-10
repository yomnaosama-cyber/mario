#ifndef LEVEL2_H
#define LEVEL2_H

#include "Level.h"

class Level2 : public Level {
public:
    Level2(int rows, int cols);
    void createTiles() override;
};

#endif