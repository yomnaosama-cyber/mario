#ifndef LEVEL_H
#define LEVEL_H
#include <vector>
#include <utility>
#include "Tile.h"
#include "Enemy.h"

using namespace std;

class Level{
  protected:
  Tile** grid;
  long spawnX;
  long spawnY;
  long flagX;
  long flagY;
  vector<Enemy*> enemy;
  int rows;
  int cols;
  
  public:

  Level(int rows, int cols);  // NEW: Simple constructor
  Level(Tile** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols);

  pair<long, long> getSpawn();
  pair<long, long> getflag();
  Tile** getgrid();
  vector<Enemy*> getenemy();
  Tile getTileAt(int row, int col);
  void createTiles();
  void createLevel3Tiles(); 
  if (isLevel3) {
    currentLevel->createLevel3Tiles();   
} else {
    currentLevel->createTiles();
}





};

#endif