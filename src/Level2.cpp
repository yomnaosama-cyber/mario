#include "Level2.h"
#include <QDebug>

Level2::Level2(int rows, int cols) : Level(rows, cols) {
    // Optional art "level2_bonus.jpg"; if missing, meadow tint from backgroundColor
    backgroundImagePath = QStringLiteral("level2_bonus.jpg");
    backgroundColor = QColor(175, 220, 185);
}

void Level2::createTiles() {
    grid = new Tile**[rows];
    for(int i = 0; i < rows; i++) {
        grid[i] = new Tile*[cols];
    }

    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            grid[i][n] = new Tile(n, i, Tile::Empty);
        }
    }

    int groundLevel = rows - 2;
    if (groundLevel < 1) groundLevel = 1;

    // Solid ground
    for(int n = 0; n < cols; n++) {
        delete grid[groundLevel][n];
        delete grid[groundLevel+1][n];
        grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);
        grid[groundLevel+1][n] = new Tile(n, groundLevel+1, Tile::Mud);
    }

    // Floating platforms
    for(int i = 0; i < 5; i++) {
        if (groundLevel-3 >= 0)
            delete grid[groundLevel-3][12 + i];
            grid[groundLevel-3][12 + i] = new Tile(12 + i, groundLevel-3, Tile::Brick);
    }
    
    for(int i = 0; i < 4; i++) {
        if (groundLevel-4 >= 0)
            delete grid[groundLevel-4][25 + i];
            grid[groundLevel-4][25 + i] = new Tile(25 + i, groundLevel-4, Tile::Brick);
    }
    
    for(int i = 0; i < 6; i++) {
        if (groundLevel-2 >= 0)
            delete grid[groundLevel-2][40 + i];
            grid[groundLevel-2][40 + i] = new Tile(40 + i, groundLevel-2, Tile::Brick);
    }
    
    for(int i = 0; i < 3; i++) {
        if (groundLevel-5 >= 0)
            delete grid[groundLevel-5][55 + i];
            grid[groundLevel-5][55 + i] = new Tile(55 + i, groundLevel-5, Tile::Brick);
    }

    // Staircase
    for(int i = 0; i < 6; i++) {
        int stepRow = groundLevel - i;
        if (stepRow >= 0)
            delete grid[stepRow][68 + i];
            grid[stepRow][68 + i] = new Tile(68 + i, stepRow, Tile::Brick);
    }

    flagX = 74;
    flagY = groundLevel - 6;
    if (flagY >= 0 && flagX < cols) {
        delete grid[flagY][flagX];
        grid[flagY][flagX] = new Tile(flagX, flagY, Tile::Flag);
    }

    spawnX = 2;
    spawnY = groundLevel;

    enemy.push_back(new Enemy(10, groundLevel, 5, 20, 0.12f));
    enemy.push_back(new Enemy(30, groundLevel, 25, 38, 0.15f));
    enemy.push_back(new Enemy(50, groundLevel, 42, 60, 0.2f));

    qDebug() << "Level 2 created";
}
