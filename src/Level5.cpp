#include "Level5.h"
#include "CrumblingTile.h"
#include <QDebug>
#include <algorithm>

Level5::Level5(int rows, int cols) : Level(rows, cols), isBossFightActive(false) {
    backgroundImagePath = "sky2.jpg";
    
    // Custom tile colors for Level 5 (fire theme)
    tileColors[Tile::Brick] = QColor(89, 45, 32);        // Dark red
    tileColors[Tile::Grass] = QColor(55, 68, 39);     
    tileColors[Tile::Mud] = QColor(61, 43, 31);      // Dim gray
}

void Level5::createTiles() {
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
    
    // Section 1: Early platforms with crumbling floor
    for(int n = 0; n < 25; n++) {
        if (n < 5 || (n >= 8 && n <= 11) || (n >= 14 && n <= 25)) {
            delete grid[groundLevel][n];
            grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);
            
            delete grid[groundLevel+1][n];
            grid[groundLevel+1][n] = new Tile(n, groundLevel+1, Tile::Mud);
        }
    }

    // Add crumbling tiles in the early section gaps
    if (groundLevel >= 2) {
        delete grid[groundLevel - 2][6];
        grid[groundLevel - 2][6] = new CrumblingTile(6, groundLevel - 2);
        
        delete grid[groundLevel - 2][7];
        grid[groundLevel - 2][7] = new CrumblingTile(7, groundLevel - 2);
        
        delete grid[groundLevel - 2][12];
        grid[groundLevel - 2][12] = new CrumblingTile(12, groundLevel - 2);
        
        delete grid[groundLevel - 2][13];
        grid[groundLevel - 2][13] = new CrumblingTile(13, groundLevel - 2);
    }

    // Section 2: Platforming section with larger gaps
    // Gap 1
    for(int n = 26; n < 30; n++) {
        delete grid[groundLevel][n];
        grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Empty);
    }
    
    // Platform after gap 1
    for(int n = 30; n < 35; n++) {
        delete grid[groundLevel][n];
        grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);
        
        delete grid[groundLevel+1][n];
        grid[groundLevel+1][n] = new Tile(n, groundLevel+1, Tile::Mud);
    }

    // Section 3: Crumbling floor challenge (cols 35-41) — triggers via checkCollisions4
    for(int n = 35; n < 42; n++) {
        if (groundLevel >= 1) {
            delete grid[groundLevel][n];
            grid[groundLevel][n] = new CrumblingTile(static_cast<long long>(n), groundLevel);
        }
    }

    // Gap 2
    for(int n = 42; n < 47; n++) {
        delete grid[groundLevel][n];
        grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Empty);
    }

    // Bowser arena: solid ground — Bowser stands on real floor, nothing crumbles here
    for(int n = 47; n < cols; n++) {
        delete grid[groundLevel][n];
        grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);

        delete grid[groundLevel+1][n];
        grid[groundLevel+1][n] = new Tile(n, groundLevel+1, Tile::Mud);
    }

    // Add floating platforms for navigation
    int platRow1 = groundLevel - 3;
    if (platRow1 >= 0) {
        for (int n = 9; n <= 11; n++) {
            delete grid[platRow1][n];
            grid[platRow1][n] = new Tile(n, platRow1, Tile::Brick);
        }
    }

    // Platform 2: crumbling bridge before gap 2 — must cross quickly!
    int platRow2 = groundLevel - 2;
    if (platRow2 >= 0) {
        for (int n = 18; n <= 22; n++) {
            delete grid[platRow2][n];
            grid[platRow2][n] = new CrumblingTile(static_cast<long long>(n), platRow2);
        }
    }

    int platRow3 = groundLevel - 3;
    if (platRow3 >= 0) {
        for (int n = 32; n <= 34; n++) {
            delete grid[platRow3][n];
            grid[platRow3][n] = new Tile(n, platRow3, Tile::Brick);
        }
    }

    // Bowser arena: plain crumbling ground only — no extra solid platform under Bowser

    // Spawn point
    spawnX = 2;
    spawnY = groundLevel;

    // Mid section - enemy in crumbling floor area
    enemy.push_back(new Enemy(38, groundLevel, 35, 42, 0.12f));   // Patrols crumbling area

    // Win zone for Bowser area
    int winX = 92;
    int winY = groundLevel;
    if (winY >= 0 && winX < cols) {
        delete grid[winY][winX];
        grid[winY][winX] = new Tile(winX, winY, Tile::Flag);
    }

    qDebug() << "Level 5 (Final Level with Bowser) created - Enhanced design";
}