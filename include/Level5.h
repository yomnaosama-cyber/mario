#ifndef LEVEL5_H
#define LEVEL5_H

#include "Level.h"

class Level5 : public Level {
private:
    bool isBossFightActive;
    
public:
    Level5(int rows, int cols);
    void createTiles() override;
    bool getIsBossFightActive() const { return isBossFightActive; }
    void setBossFightActive(bool active) { isBossFightActive = active; }
};

#endif