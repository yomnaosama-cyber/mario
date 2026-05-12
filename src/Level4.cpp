#include "Level4.h"

void Level4::createTiles() {
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
            grid[groundLevel][n] = Tile(n, groundLevel, Tile::Grass);
        }
        else{
            grid[groundLevel][n] = Tile(n, groundLevel, Tile::Empty);

            Ct.push_back(new CrumblingTile(n, groundLevel-2));


        }


    }

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
                grid[i][n] = Tile(n, i, Tile::Mud);
            } else {
                grid[i][n] = Tile(n, i, Tile::Empty);
            }
        }
    }



    // First platform moved two rows higher than before
    int firstPlatformRow = groundLevel - 3;
    int platformRow1 = groundLevel - 1;





    // First platform (low,Changed all platforms to moving)
    for(int i=8; i < 13; ++i){
        grid[firstPlatformRow][i] = Tile(i, firstPlatformRow, Tile::Brick);
        Mp.push_back(new MovingPlatform(i,firstPlatformRow,2,i-2,i+2, Horizontal));
    }
    // Second platform (gap to jump over)

    for(int i=20; i < 23; ++i){
        grid[platformRow1][i] = Tile(i, platformRow1, Tile::Brick);
        Mp.push_back(new MovingPlatform(i,platformRow1,2,i-2,i+2, Horizontal));
    }

    // Third platform (higher up)
    int platformRow2 = groundLevel - 3;
    if (platformRow2 >= 0) {
        grid[platformRow2][35] = Tile(35, platformRow2, Tile::Brick);
        grid[platformRow2][36] = Tile(36, platformRow2, Tile::Brick);
        grid[platformRow2][37] = Tile(37, platformRow2, Tile::Brick);
        grid[platformRow2][38] = Tile(38, platformRow2, Tile::Brick);
    }

    grid[platformRow2+1][37] = SecretCoinTile(37, platformRow2+1);



    //Underground section
        //Creating Empty tiles where the underground will be
    int tunnel = groundLevel + 2;
    int tunnelend = rows-1;
    for(int i = tunnel; tunnelend > i;++i){

        for(int n = 40 ; n < 63; n++){
            grid[i][n]= Tile(n,i, Tile::Empty);

        }
    }

    // Crumbling Tile and Moving Platform before the first key coin
    Ct.push_back(new CrumblingTile(42, tunnel+5));
    Mp.push_back(new MovingPlatform(43,tunnel+4,2,41,46,Horizontal));

    //Elevator down to the tunnel
    Mp.push_back(new MovingPlatform(41, groundLevel-1,2,groundLevel-2, tunnelend-2,Vertical));

    //Crumbling Tile and Moving Platform for second secret key coin
    Mp.push_back(new MovingPlatform(49,rows-4,2,tunnel+2,rows-3,Vertical));
    Ct.push_back(new CrumblingTile(50, tunnel+3));

    //For third key coin
    Ct.push_back(new CrumblingTile(52, rows - 2));
    Ct.push_back(new CrumblingTile(53, rows - 3));
    Ct.push_back(new CrumblingTile(54, rows - 4));

    //Secret key coins
    grid[tunnel + 3][44] = SecretCoinTile(44,tunnel + 3 );
    grid[tunnel + 2][50] = SecretCoinTile(50,tunnel + 2 );
    grid[tunnel + 1][54] = SecretCoinTile(54,tunnel + 1 );


    //Elevator back up from the tunnel
    Mp.push_back(new MovingPlatform(60, tunnelend-2,2,tunnelend-1,groundLevel-2 ,Vertical));



    //LockedDoor where Mario has to have the required keys to pass
    ld = new LockedDoor(63,groundLevel,Tile::Door,5);
    grid[groundLevel][63] =  *ld;

    // Staircase to flag - continuous steps that rest on ground
    if (groundLevel-1 >= 0) grid[groundLevel-1][65] = Tile(65, groundLevel-1, Tile::Brick);
    if (groundLevel-2 >= 0) grid[groundLevel-2][68] = Tile(68, groundLevel-2, Tile::Brick);
    if (groundLevel-3 >= 0) grid[groundLevel-3][71] = Tile(71, groundLevel-3, Tile::Brick);
    if (groundLevel-4 >= 0) grid[groundLevel-4][74] = Tile(74, groundLevel-4, Tile::Brick);
    //if (groundLevel-5 >= 0) grid[groundLevel-5][67] = Tile(67, groundLevel-5, Tile::Brick);

    // Flag: place it close to the last stair step so it is reachable
    flagX = 76;
    flagY = groundLevel - 4;
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
