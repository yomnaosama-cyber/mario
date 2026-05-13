#include <vector>
#include <QDebug>
#include "Level.h"
using namespace std;

Level::Level(int rows, int cols)
    : grid(nullptr), spawnX(2), spawnY(0), flagX(cols-5), flagY(0), rows(rows), cols(cols), backgroundColor(), backgroundImagePath() {
    enemy.clear();
    tileColors[Tile::Brick] = QColor(160, 82, 45);
    tileColors[Tile::Grass] = QColor(34, 139, 34);
    tileColors[Tile::Mud] = QColor(101, 67, 33);
    tileColors[Tile::Steel] = QColor(169, 169, 169);
    tileColors[Tile::MysteryBox] = QColor(218, 165, 32);
    tileColors[Tile::Flag] = QColor(255, 0, 0);
    tileColors[Tile::Empty] = QColor(139, 69, 19);
    tileColors[Tile::Door] = QColor(101, 67, 33);
}

Level::Level(Tile*** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols)
    : grid(grid), spawnX(spawnX), spawnY(spawnY), flagX(flagX), flagY(flagY), rows(rows), cols(cols), backgroundColor(), backgroundImagePath() {
    tileColors[Tile::Brick] = QColor(160, 82, 45);
    tileColors[Tile::Grass] = QColor(34, 139, 34);
    tileColors[Tile::Mud] = QColor(101, 67, 33);
    tileColors[Tile::Steel] = QColor(169, 169, 169);
    tileColors[Tile::MysteryBox] = QColor(218, 165, 32);
    tileColors[Tile::Flag] = QColor(255, 0, 0);
    tileColors[Tile::Empty] = QColor(139, 69, 19);
    tileColors[Tile::Door] = QColor(101, 67, 33);
}

Level::~Level() {
    if (grid) {
        for (int i = 0; i < rows; i++) {
            for(int j=0; j <cols; j++){
                delete grid[i][j];
            }
            delete[] grid[i];
        }
        delete[] grid;
    }
    // Enemy pointers are owned by the GameController, not the Level object.
    enemy.clear();
}

pair<long, long> Level::getSpawn() {
    return make_pair(spawnX * 50, spawnY * 50);
}

QColor Level::getBackgroundColor() const {
    return backgroundColor;
}

QString Level::getBackgroundImagePath() const {
    return backgroundImagePath;
}

QColor Level::getTileColor(Tile::Tiletype type) const {
    const int idx = static_cast<int>(type);
    if (idx >= 0 && idx < 8) {
        return tileColors[idx];
    }
    return QColor(139, 69, 19);
}

pair<long, long> Level::getflag() {
    return make_pair(flagX * 50, flagY * 50);
}

Tile*** Level::getgrid() {
    return grid;
}

vector<Enemy*> Level::getenemy() {
    return enemy;
}

Tile* Level::getTileAt(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols && grid) {
        return grid[row][col];
    }
    return nullptr;
}

/*void Level::setTile(int row, int col){
    if (row >= 0 && row < rows && col >= 0 && col < cols) {

        if(grid[row][col]){

            if(grid[row][col]->scene()){
                grid[row][col]->scene()
            }
        }
    }
}*/
