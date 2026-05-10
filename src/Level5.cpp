#include "Level5.h"
#include <QDebug>

Level5::Level5(int rows, int cols) : Level(rows, cols), isBossFightActive(false) {
    backgroundImagePath = "sky2.jpg";
    
    // Custom tile colors for Level 5 (fire theme)
    tileColors[Tile::Brick] = QColor(89, 45, 32);        // Dark red
    tileColors[Tile::Grass] = QColor(55, 68, 39);     
    tileColors[Tile::Mud] = QColor(61, 43, 31);      // Dim gray
}

void Level5::createTiles() {
    grid = new Tile*[rows];
    for(int i = 0; i < rows; i++) {
        grid[i] = new Tile[cols];
    }

    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            grid[i][n] = Tile(n, i, Tile::Empty);
        }
    }

    int groundLevel = rows - 2;
    if (groundLevel < 1) groundLevel = 1;

    // Ground floor only
    for(int n = 0; n < cols; n++) {
        grid[groundLevel][n] = Tile(n, groundLevel, Tile::Grass);
        grid[groundLevel+1][n] = Tile(n, groundLevel+1, Tile::Mud);
    }

    // One hole to fall from
    int holeColumn = 20;
    if (holeColumn >= 0 && holeColumn < cols) {
        grid[groundLevel][holeColumn] = Tile(holeColumn, groundLevel, Tile::Empty);
        grid[groundLevel+1][holeColumn] = Tile(holeColumn, groundLevel+1, Tile::Empty);
    }

    // One original enemy on this final level
    enemy.push_back(new Enemy(12, groundLevel, 8, 16, 0.20f));

    // Win zone at the end of the ground
    int winX = 92;
    int winY = groundLevel;
    if (winY >= 0 && winX < cols) {
        grid[winY][winX] = Tile(winX, winY, Tile::Flag);
    }

    spawnX = 2;
    spawnY = groundLevel;
    
    // Bowser will be added manually by GameController
    qDebug() << "Level 5 (Final Level with Bowser) created";
}