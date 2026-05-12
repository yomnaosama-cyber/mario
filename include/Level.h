#ifndef LEVEL_H
#define LEVEL_H
#include <vector>
#include <utility>
#include <QColor>
#include <QString>
#include "Tile.h"
#include "Enemy.h"

using namespace std;

class Level {
protected:
    Tile** grid;
    long spawnX;
    long spawnY;
    long flagX;
    long flagY;
    vector<Enemy*> enemy;
    int rows;
    int cols;
    QColor backgroundColor;
    QString backgroundImagePath;
    QColor tileColors[7];
    
public:
    Level(int rows, int cols);
    Level(Tile** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols);
    virtual ~Level();

<<<<<<< HEAD
    pair<long, long> getSpawn();
    pair<long, long> getflag();
    Tile** getgrid();
    vector<Enemy*> getenemy();
    Tile getTileAt(int row, int col);
    QColor getBackgroundColor() const;
    QString getBackgroundImagePath() const;
    QColor getTileColor(Tile::Tiletype type) const;
    
    virtual void createTiles() = 0;
    QColor* getTileColors() { return tileColors; }
=======
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





>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
};

#endif