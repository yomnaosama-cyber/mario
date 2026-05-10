#ifndef LEVEL3_H
#define LEVEL3_H

#include "Level.h"

class Level3 : public Level {
public:
    Level3(int rows, int cols);
    void createTiles() override;
};

#endif