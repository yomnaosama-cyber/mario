#ifndef LEVEL4_H
#define LEVEL4_H

#include "Level.h"
#include "Tile.h"
#include "movingplatform.h"
#include "CrumblingTile.h"

class Level4 : public Level{

    vector<MovingPlatform*> Mp;
    vector<CrumblingTile*> Ct;
    LockedDoor* ld;
    int keycount;

public:
    Level4(int rows, int cols);
    Level4(Tile** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols);
    vector<MovingPlatform*> getMovingPlat();
    vector<CrumblingTile*> getCrumblingTil();
    LockedDoor* getLockedDoor();
    int getkeycount();

    void createTiles() override;
};

#endif // LEVEL4_H
