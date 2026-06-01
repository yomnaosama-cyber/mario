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
    friend class GameController;
protected:
    Tile*** grid;
    long spawnX;
    long spawnY;
    long flagX;
    long flagY;
    vector<Enemy*> enemy;
    int rows;
    int cols;
    QColor backgroundColor;
    QString backgroundImagePath;
    QColor tileColors[8];

public:
    Level(int rows, int cols);
    Level(Tile*** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols);
    virtual ~Level();

    pair<long, long> getSpawn();
    pair<long, long> getflag();
    Tile*** getgrid();
    vector<Enemy*> getenemy();
    Tile* getTileAt(int row, int col);
    int getRowCount() const { return rows; }
    int getColCount() const { return cols; }
    QColor getBackgroundColor() const;
    QString getBackgroundImagePath() const;
    QColor getTileColor(Tile::Tiletype type) const;
    void setTile(int row, int col, Tile* newTile);

    virtual void createTiles() = 0;
    QColor* getTileColors() { return tileColors; }
};



#endif
