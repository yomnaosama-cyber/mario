#include <vector>
#include <QDebug>
#include "Level.h"
using namespace std;

Level::Level(int rows, int cols) 
    : grid(nullptr), spawnX(2), spawnY(0), flagX(cols-5), flagY(0), rows(rows), cols(cols) {
    enemy.clear();
}

Level::Level(Tile** grid, long spawnX, long spawnY, long flagX, long flagY, int rows, int cols) 
    : grid(grid), spawnX(spawnX), spawnY(spawnY), flagX(flagX), flagY(flagY), rows(rows), cols(cols) {
}

pair<long, long> Level::getSpawn() {
    return make_pair(spawnX * 50, spawnY * 50);  // FIXED: Convert to pixels
}

pair<long, long> Level::getflag() {
    return make_pair(flagX * 50, flagY * 50);    // FIXED: Convert to pixels
}

Tile** Level::getgrid() {
    return grid;
}

vector<Enemy*> Level::getenemy() {
    return enemy;
}

Tile Level::getTileAt(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return grid[row][col];
    }
    return Tile();
}

void Level::createTiles() {
    // Allocate grid
    grid = new Tile*[rows];
    for(int i = 0; i < rows; i++) {
        grid[i] = new Tile[cols];
    }

    // Initialize all tiles to empty
    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            grid[i][n] = Tile(n, i, Tile::Empty);
        }
    }

    // Two-layer floor: grass on top, mud underneath
    int groundLevel = rows - 2;
    if (groundLevel < 1) {
        groundLevel = 1;
    }

    // Fill the main ground row with Grass (solid) with a couple of gaps Mario can fall through
    for(int n = 0; n < cols; n++) {
        // Create two gaps in the floor
        bool isGap =
            // First gap
            (n >= 15 && n <= 18) ||
            // Second gap
            (n >= 32 && n <= 35);

        if (!isGap) {
            grid[groundLevel][n] = Tile(n, groundLevel, Tile::Grass);
        } else {
            grid[groundLevel][n] = Tile(n, groundLevel, Tile::Empty);
        }
    }

    // Add a supporting Mud row directly under the grass
    int mudRow = groundLevel + 1;
    for (int n = 0; n < cols; n++) {
        bool isGap =
            (n >= 15 && n <= 18) ||
            (n >= 32 && n <= 35);
        if (!isGap) {
            grid[mudRow][n] = Tile(n, mudRow, Tile::Mud);
        } else {
            grid[mudRow][n] = Tile(n, mudRow, Tile::Empty);
        }
    }

    // First platform moved two rows higher than before
    int firstPlatformRow = groundLevel - 3;
    int platformRow1 = groundLevel - 1;
    
    // First platform (low, near start)
    grid[firstPlatformRow][8] = Tile(8, firstPlatformRow, Tile::Brick);
    grid[firstPlatformRow][9] = Tile(9, firstPlatformRow, Tile::Brick);
    grid[firstPlatformRow][10] = Tile(10, firstPlatformRow, Tile::Brick);
    
    // Second platform (gap to jump over)
    grid[platformRow1][20] = Tile(20, platformRow1, Tile::Brick);
    grid[platformRow1][21] = Tile(21, platformRow1, Tile::Brick);
    grid[platformRow1][22] = Tile(22, platformRow1, Tile::Brick);
    
    // Third platform (higher up)
    int platformRow2 = groundLevel - 3;
    if (platformRow2 >= 0) {
        grid[platformRow2][35] = Tile(35, platformRow2, Tile::Brick);
        grid[platformRow2][36] = Tile(36, platformRow2, Tile::Brick);
        grid[platformRow2][37] = Tile(37, platformRow2, Tile::Brick);
        grid[platformRow2][38] = Tile(38, platformRow2, Tile::Brick);
    }
    
    // Staircase to flag - continuous steps that rest on ground
    if (groundLevel-1 >= 0) grid[groundLevel-1][55] = Tile(55, groundLevel-1, Tile::Brick);
    if (groundLevel-2 >= 0) grid[groundLevel-2][58] = Tile(58, groundLevel-2, Tile::Brick);
    if (groundLevel-3 >= 0) grid[groundLevel-3][61] = Tile(61, groundLevel-3, Tile::Brick);
    if (groundLevel-4 >= 0) grid[groundLevel-4][64] = Tile(64, groundLevel-4, Tile::Brick);
    if (groundLevel-5 >= 0) grid[groundLevel-5][67] = Tile(67, groundLevel-5, Tile::Brick);

    // Flag: place it close to the last stair step so it is reachable
    flagX = 69;
    flagY = groundLevel - 5;
    if (flagY >= 0 && flagX >= 0 && flagX < cols) {
        grid[flagY][flagX] = Tile(flagX, flagY, Tile::Flag);
    }

    // Spawn - Mario starts on the ground
    spawnX = 2;
    spawnY = groundLevel;

    // Enemies: keep patrols away from floor gaps so they do not float over holes
    enemy.push_back(new Enemy(12, groundLevel, 8, 14, 0.1f));
    enemy.push_back(new Enemy(42, groundLevel, 36, 50, 0.25f));

    qDebug() << "Ground level row:" << groundLevel;
    qDebug() << "Total rows:" << rows << "Total cols:" << cols;
    
    // Count non-empty tiles for debugging
    int tileCount = 0;
    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            if (grid[i][n].getType() != Tile::Empty) {
                tileCount++;
            }
        }
    }
    qDebug() << "Total non-empty tiles created:" << tileCount;
}