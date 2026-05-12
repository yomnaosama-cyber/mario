#include "Level1.h"
#include <QDebug>

Level1::Level1(int rows, int cols) : Level(rows, cols) {
    backgroundImagePath = "sky1.jpg";
}

void Level1::createTiles() {
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

    int groundLevel = rows - 2;
    if (groundLevel < 1) groundLevel = 1;

    // Ground with gaps
    for(int n = 0; n < cols; n++) {
        bool isGap = (n >= 15 && n <= 18) || (n >= 32 && n <= 35);
        if (!isGap) {
            grid[groundLevel][n] = Tile(n, groundLevel, Tile::Grass);
        }
    }

    // Mud layer
    int mudRow = groundLevel + 1;
    for (int n = 0; n < cols; n++) {
        bool isGap = (n >= 15 && n <= 18) || (n >= 32 && n <= 35);
        if (!isGap) {
            grid[mudRow][n] = Tile(n, mudRow, Tile::Mud);
        }
    }

    // Platforms
    if (groundLevel-3 >= 0) {
        grid[groundLevel-3][8] = Tile(8, groundLevel-3, Tile::Brick);
        grid[groundLevel-3][9] = Tile(9, groundLevel-3, Tile::Brick);
        grid[groundLevel-3][10] = Tile(10, groundLevel-3, Tile::Brick);
    }
    
    if (groundLevel-1 >= 0) {
        grid[groundLevel-1][20] = Tile(20, groundLevel-1, Tile::Brick);
        grid[groundLevel-1][21] = Tile(21, groundLevel-1, Tile::Brick);
        grid[groundLevel-1][22] = Tile(22, groundLevel-1, Tile::Brick);
    }
    
    if (groundLevel-3 >= 0) {
        grid[groundLevel-3][35] = Tile(35, groundLevel-3, Tile::Brick);
        grid[groundLevel-3][36] = Tile(36, groundLevel-3, Tile::Brick);
        grid[groundLevel-3][37] = Tile(37, groundLevel-3, Tile::Brick);
        grid[groundLevel-3][38] = Tile(38, groundLevel-3, Tile::Brick);
    }

    // Staircase
    if (groundLevel-1 >= 0) grid[groundLevel-1][55] = Tile(55, groundLevel-1, Tile::Brick);
    if (groundLevel-2 >= 0) grid[groundLevel-2][58] = Tile(58, groundLevel-2, Tile::Brick);
    if (groundLevel-3 >= 0) grid[groundLevel-3][61] = Tile(61, groundLevel-3, Tile::Brick);
    if (groundLevel-4 >= 0) grid[groundLevel-4][64] = Tile(64, groundLevel-4, Tile::Brick);
    if (groundLevel-5 >= 0) grid[groundLevel-5][67] = Tile(67, groundLevel-5, Tile::Brick);

    flagX = 69;
    flagY = groundLevel - 5;
    if (flagY >= 0 && flagX >= 0 && flagX < cols) {
        grid[flagY][flagX] = Tile(flagX, flagY, Tile::Flag);
    }

    spawnX = 2;
    spawnY = groundLevel;

    enemy.push_back(new Enemy(12, groundLevel, 8, 14, 0.1f));
    enemy.push_back(new Enemy(42, groundLevel, 36, 50, 0.25f));

    qDebug() << "Level 1 created - Ground row:" << groundLevel;
}