#include "Level4.h"
#include <algorithm>

Level4::Level4(int rows, int cols) : Level(rows, cols), ld(nullptr), keycount(0) {
    // Tunnel / cave mood; optional texture "level4_tunnel.jpg" in assets
    backgroundImagePath = QStringLiteral("level4_tunnel.jpg");
    backgroundColor = QColor(48, 52, 68);
}

void Level4::createTiles() {
    const int TS = 50; // must match GameController tileSize / Level::getSpawn()

    const auto inGrid = [this](int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    };

    // Allocate grid
    grid = new Tile**[rows];
    for(int i = 0; i < rows; i++) {
        grid[i] = new Tile*[cols];
    }

    // Initialize all tiles to empty
    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            grid[i][n] = new Tile(n, i, Tile::Empty);
        }
    }

    // Two-layer floor: grass on top, mud underneath
    int groundLevel = rows * 2 /3;
    if (groundLevel < 1) {
        groundLevel = 1;
    }

    // Fill the main ground row with Grass (solid) with a couple of gaps Mario can fall through
    for(int n = 0; n < cols; n++) {
        // Create 3 gaps in the floor
        bool isGap =
            // First gap
            (n >= 4 && n <= 7) ||
            // Second gap
            (n >= 15 && n <= 18) ||
            //Third gap
            (n >= 30 && n<=33);



        if (!isGap) {
            delete grid[groundLevel][n];
            grid[groundLevel][n] = new Tile(n, groundLevel, Tile::Grass);
        } else if (groundLevel >= 2) {
            delete grid[groundLevel - 2][n];
            grid[groundLevel - 2][n] = new CrumblingTile(static_cast<long long>(n), groundLevel - 2);
        }



    }

    //Secret Key Coins
    delete grid[groundLevel - 4][5];
    grid[groundLevel-4][5] = new SecretCoinTile(static_cast<long long>(5), groundLevel - 4);


    delete grid[groundLevel - 5][17];
    grid[groundLevel-5][17] = new SecretCoinTile(static_cast<long long>(17), groundLevel - 5);

    // Add a supporting Mud row directly under the grass
    int mudRow = groundLevel + 1;
    for (int n = 0; n < cols; n++) {
        for(int i = mudRow; i < rows; i++){
            bool isGap =
                // First gap
                (n >= 4 && n <= 7) ||
                // Second gap
                (n >= 15 && n <= 18) ||
                //Third gap
                (n >= 30 && n<=33);
            if (!isGap) {
                delete grid[i][n];
                grid[i][n] = new Tile(n, i, Tile::Mud);
            } /*else {
                grid[i][n] = Tile(n, i, Tile::Empty);
            }*/
        }
    }



    // First platform moved two rows higher than before
    int firstPlatformRow = groundLevel - 3;
    //int platformRow1 = groundLevel - 1;





    // First platform (low,Changed all platforms to moving)
    if (firstPlatformRow >= 0 && firstPlatformRow < rows) {
        for (int i = 10; i < 12 && i < cols; ++i) {
            if (!inGrid(firstPlatformRow, i)) {
                continue;
            }
            delete grid[firstPlatformRow][i];
            grid[firstPlatformRow][i] = new MovingPlatform(i, firstPlatformRow, 2, (i - 2) * TS, (i + 2) * TS, MovingPlatform::Horizontal);
        }
    }
    // Second platform (gap to jump over)

    /*
    if (platformRow1 >= 0 && platformRow1 < rows) {
        for (int i = 20; i < 22 && i < cols; ++i) {
            if (!inGrid(platformRow1, i)) {
                continue;
            }
            delete grid[platformRow1][i];
            grid[platformRow1][i] = new MovingPlatform(i, platformRow1, 2, (i - 2) * TS, (i + 2) * TS, MovingPlatform::Horizontal);
        }
    }*/

    // Third platform (higher up)
    int platformRow2 = groundLevel - 3;
    if (platformRow2 >= 0 && platformRow2 < rows) {
        for (int bc = 35; bc <= 37; ++bc) {
            if (inGrid(platformRow2, bc)) {
                delete grid[platformRow2][bc];
                grid[platformRow2][bc] = new MovingPlatform(bc, platformRow2,3, (bc - 2) * TS, (bc +2) * TS, MovingPlatform::Horizontal);
            }
        }
    }

    const int secretRow = platformRow2 + 1;
    if (secretRow >= 0 && secretRow < rows && 37 < cols) {
        delete grid[secretRow][37];
        grid[secretRow][37] = new SecretCoinTile(37, secretRow);
    }



    //Underground section
        //Creating Empty tiles where the underground will be
    int tunnel = groundLevel + 2;
    int tunnelend = rows - 1;
    const int tunnelColEnd = (std::min)(cols, 63);
    for (int i = tunnel; tunnelend > i && i < rows; ++i) {
        for (int n = 40; n < tunnelColEnd; ++n) {
            if (!inGrid(i, n)) {
                continue;
            }
            delete grid[i][n];
            grid[i][n] = new Tile(n, i, Tile::Empty);
        }
    }

    //Clearing the entrance
    /*
    int entranceCol = 40;
    for (int i = groundLevel; i < tunnel; ++i) {
        if (inGrid(i, entranceCol) && inGrid(i, entranceCol + 1)) {
            // Clear first column of entrance
            delete grid[i][entranceCol];
            grid[i][entranceCol] = new Tile(entranceCol, i, Tile::Empty);

            // Clear second column of entrance (optional, makes it wider)
            delete grid[i][entranceCol + 1];
            grid[i][entranceCol + 1] = new Tile(entranceCol + 1, i, Tile::Empty);
        }
    }
    */
    // Crumbling Tile and Moving Platform before the first key coin

    /*
    if (inGrid(rows - 4 , 42)) {
        delete grid[rows-4][42];
        grid[rows-4][42] = new CrumblingTile(42, rows-4);
    }

    if (inGrid(rows - 3, 43)) {
        delete grid[rows-3][43];
        grid[rows-3][43] = new MovingPlatform(43, rows-3, 2, 41 * TS, 46 * TS, MovingPlatform::Horizontal);
    }*/


    // Clearing holes for Elevator
    for (int i = groundLevel; i <= groundLevel + 1; ++i) {
        for (int ec = 40; ec <= 42; ++ec) {
            if (inGrid(i, ec)) {
            delete grid[i][ec];
            grid[i][ec] = new Tile(ec, i, Tile::Empty);
        }
        }
    }

    //Elevator down to the tunnel
    if (inGrid(groundLevel - 1, 41)) {
        delete grid[groundLevel - 1][41];
        grid[groundLevel - 1][41] = new MovingPlatform(41, groundLevel - 1, 2, (groundLevel - 2) * TS, (tunnelend - 2) * TS, MovingPlatform::Vertical);
    }

    //Secret Key Coins

    if (inGrid(tunnel + 3, 44)) {
        delete grid[tunnel + 3][44];
        grid[tunnel + 3][44] = new SecretCoinTile(44, tunnel + 3);
    }
    if (inGrid(tunnel + 2, 50)) {
        delete grid[tunnel + 2][50];
        grid[tunnel + 2][50] = new SecretCoinTile(50, tunnel + 2);
    }
    if (inGrid(tunnel + 1, 54)) {
        delete grid[tunnel + 1][54];
        grid[tunnel + 1][54] = new SecretCoinTile(54, tunnel + 1);
    }

    //Elevator back up from the tunnel

    // Clearing holes for Elevator
    for (int i = groundLevel; i <= groundLevel + 1; ++i) {
        for (int ec = 59; ec <= 62; ++ec) {
            if (inGrid(i, ec)) {
                delete grid[i][ec];
                grid[i][ec] = new Tile(ec, i, Tile::Empty);
            }
        }
    }
    if (inGrid(tunnelend - 2, 60)) {
        delete grid[tunnelend - 2][60];
        const int rLo = (std::min)(tunnelend - 1, groundLevel - 2);
        const int rHi = (std::max)(tunnelend - 1, groundLevel - 2);
        grid[tunnelend - 2][60] = new MovingPlatform(60, tunnelend - 2, 2, rLo * TS, rHi * TS, MovingPlatform::Vertical);
    }



    //LockedDoor where Mario has to have the required keys to pass
    ld = nullptr;
    if (inGrid(groundLevel-1, 64)) {
        ld = new LockedDoor(64, groundLevel-1, Tile::Door, 5);
        delete grid[groundLevel-1][64];
        grid[groundLevel-1][64] = ld;
    }

    // Staircase to flag - continuous steps that rest on ground

    for (int i = 1; i < 5; i++) {
        int row = groundLevel - i;
        int col = 62 + (i * 3);

        if (inGrid(row, col)) {
            delete grid[row][col];
            grid[row][col] = new CrumblingTile(col, row);
        }
    }


    // Flag: place it close to the last stair step so it is reachable
    flagX = 76;
    flagY = groundLevel - 4;
    if (flagY >= 0 && flagX >= 0 && flagX < cols) {
        delete grid[flagY][flagX];
        grid[flagY][flagX] = new Tile(flagX, flagY, Tile::Flag);
    }

    // Spawn - Mario starts on the ground
    spawnX = 2;
    spawnY = groundLevel;

    // Enemies: keep patrols away from floor gaps so they do not float over holes
    enemy.push_back(new Enemy(12, groundLevel,  8, 14, 0.10f)); // between gap1(4-7) and gap2(15-18)
    enemy.push_back(new Enemy(25, groundLevel, 21, 29, 0.15f)); // between gap2(15-18) and gap3(30-33)
    enemy.push_back(new Enemy(36, groundLevel, 34, 39, 0.20f)); // between gap3(30-33) and elevator(40-42)

    qDebug() << "Ground level row:" << groundLevel;
    qDebug() << "Total rows:" << rows << "Total cols:" << cols;

    // Count non-empty tiles for debugging
    int tileCount = 0;
    for(int i = 0; i < rows; i++) {
        for(int n = 0; n < cols; n++) {
            if (grid[i][n]-> getType() != Tile::Empty) {
                tileCount++;
            }
        }
    }
    qDebug() << "Total non-empty tiles created:" << tileCount;
}

LockedDoor* Level4::getLockedDoor() {
    return ld;
}

vector<MovingPlatform*> Level4::getMovingPlat() {
    return Mp;
}

vector<CrumblingTile*> Level4::getCrumblingTil() {
    return Ct;
}

int Level4::getkeycount() {
    return keycount;
}
