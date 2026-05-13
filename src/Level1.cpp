#include "Level1.h"
#include <QDebug>

Level1::Level1(int rows, int cols) : Level(rows, cols) {
    backgroundImagePath = QStringLiteral("sky1.jpg");
    backgroundColor = QColor(173, 216, 230); // light blue if sky image is missing
}

void Level1::createTiles() {
    grid = new Tile**[rows];
    for (int i = 0; i < rows; i++) {
        grid[i] = new Tile*[cols];
        for (int n = 0; n < cols; n++){
            grid[i][n] = new Tile(n, i, Tile::Empty);
        }
    }

    int groundLevel = rows - 2;
    if (groundLevel < 1) groundLevel = 1;
    int mudRow = groundLevel + 1;

    // --- LEVEL 1 floor with DIFFERENT gap positions ---
    for (int n = 0; n < cols; n++) {
        // Three gaps: early, mid, late
        bool isGap = (n >= 10 && n <= 13) ||   // early gap
                     (n >= 28 && n <= 30) ||   // mid gap
                     (n >= 48 && n <= 52);     // wide late gap

        if (!isGap) {
            delete grid[groundLevel][n];
            grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);

            delete grid[mudRow][n];
            grid[mudRow][n] = new Tile(n, mudRow,  Tile::Mud);
        }
    }

    // --- Platforms (different layout from Level 0) ---
    // Stepping stones over the early gap

    delete grid[groundLevel-2][11];
    grid[groundLevel-2][11] = new Tile(11, groundLevel-2, Tile::Brick);

    delete grid[groundLevel-2][12];
    grid[groundLevel-2][12] = new Tile(12, groundLevel-2, Tile::Brick);

    // Floating island after first gap
    int platA = groundLevel - 4;
    if (platA >= 0) {
        for (int n = 17; n <= 21; n++){
            delete grid[platA][n];
            grid[platA][n] = new Tile(n, platA, Tile::Brick);
        }
    }

    // Mid-level platform with coins above (coins placed in GameController)
    int platB = groundLevel - 3;
    if (platB >= 0) {
        for (int n = 33; n <= 37; n++){
            delete grid[platB][n];
            grid[platB][n] = new Tile(n, platB, Tile::Brick);
        }
    }

    // High platform bridging the wide gap
    int platC = groundLevel - 5;
    if (platC >= 0) {
        for (int n = 43; n <= 46; n++){
            delete grid[platC][n];
            grid[platC][n] = new Tile(n, platC, Tile::Brick);
        }
        for (int n = 53; n <= 56; n++){
                delete grid[platC][n];
            grid[platC][n] = new Tile(n, platC, Tile::Brick);
        }
    }

    // Descending staircase to flag
    for (int step = 0; step < 6; step++) {
        int r = groundLevel - step;
        int c = 60 + step * 3;
        if (r >= 0 && c < cols){
            delete grid[r][c];
            grid[r][c] = new Tile(c, r, Tile::Brick);
        }
    }

    // Flag
    flagX = 79;
    flagY = groundLevel - 5;
    if (flagY >= 0 && flagX < cols){
        delete grid[flagY][flagX];
        grid[flagY][flagX] = new Tile(flagX, flagY, Tile::Flag);
    }

    // Spawn
    spawnX = 2;
    spawnY = groundLevel;

    // Enemies - different patrol zones than Level 0
    enemy.push_back(new Enemy(7,  groundLevel, 4,  9,  0.15f));
    enemy.push_back(new Enemy(22, groundLevel, 14, 27, 0.2f));
    enemy.push_back(new Enemy(40, groundLevel, 35, 47, 0.18f));

    qDebug() << "Level1 created. Ground:" << groundLevel;
}
