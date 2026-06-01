#include "Level3.h"
#include <QDebug>
#include "PiranhaPlant.h"

Level3::Level3(int rows, int cols) : Level(rows, cols) {
    backgroundImagePath.clear();
    backgroundColor = QColor(60, 110, 150); // fallback under level3.png
}

void Level3::createTiles() {

    // Allocate grid
    grid = new Tile** [rows];
    for (int i = 0; i < rows; i++) {
        grid[i] = new Tile*[cols];
    }

    // Fill empty
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = new Tile(j, i, Tile::Empty);
        }
    }

    // Ground (solid throughout — Level 3 has no floor gaps)
    int ground = rows - 2;
    for (int j = 0; j < cols; j++) {
        delete grid[ground][j];
        grid[ground][j] = new Tile(j, ground, Tile::Grass);
        if (ground + 1 < rows) {
            delete grid[ground + 1][j];
            grid[ground + 1][j] = new Tile(j, ground + 1, Tile::Mud);
        }
    }

    // ── Platform layout — progressively higher, matching Incorporate design ──

    // Platform 1: low stepping stone (early game)
    for (int j = 6; j < 12; j++) {
        if (ground - 3 >= 0) {
            delete grid[ground - 3][j];
            grid[ground - 3][j] = new Tile(j, ground - 3, Tile::Brick);
        }
    }

    // Platform 2: mid-height jump
    for (int j = 18; j < 24; j++) {
        if (ground - 5 >= 0) {
            delete grid[ground - 5][j];
            grid[ground - 5][j] = new Tile(j, ground - 5, Tile::Brick);
        }
    }

    // Platform 3: same height as platform 1, gap to leap
    for (int j = 30; j < 36; j++) {
        if (ground - 3 >= 0) {
            delete grid[ground - 3][j];
            grid[ground - 3][j] = new Tile(j, ground - 3, Tile::Brick);
        }
    }

    // Platform 4: high platform above the piranha pipe area
    for (int j = 42; j < 48; j++) {
        if (ground - 5 >= 0) {
            delete grid[ground - 5][j];
            grid[ground - 5][j] = new Tile(j, ground - 5, Tile::Brick);
        }
    }

    // Platform 5: approach to the flag
    for (int j = 55; j < 62; j++) {
        if (ground - 3 >= 0) {
            delete grid[ground - 3][j];
            grid[ground - 3][j] = new Tile(j, ground - 3, Tile::Brick);
        }
    }

    // Staircase leading up to the flag
    for (int step = 0; step < 5; step++) {
        int r = ground - step;
        int c = 65 + step * 2;
        if (r >= 0 && c < cols) {
            delete grid[r][c];
            grid[r][c] = new Tile(c, r, Tile::Brick);
        }
    }

    // Spawn
    spawnX = 2;
    spawnY = ground;

    // Flag — at the top of the staircase
    flagX = cols - 4;
    flagY = (ground - 5 >= 0) ? ground - 5 : ground;
    if (flagX >= 0 && flagX < cols && flagY >= 0 && flagY < rows) {
        delete grid[flagY][flagX];
        grid[flagY][flagX] = new Tile(flagX, flagY, Tile::Flag);
    }

    // ── Enemies ──────────────────────────────────────────────────────────

    enemy.clear();

    // Ground patrol enemies distributed across the level
    enemy.push_back(new Enemy(9,  ground, 6,  14, 0.20f));
    enemy.push_back(new Enemy(21, ground, 16, 28, 0.22f));
    enemy.push_back(new Enemy(33, ground, 28, 40, 0.25f));
    enemy.push_back(new Enemy(50, ground, 44, 58, 0.28f));

    // Piranha Plant at column 25 (matching original Incorporate.cpp position)
    enemy.push_back(new PiranhaPlant(25, rows - 2));

    qDebug() << "Level 3 created. Ground:" << ground;
}
