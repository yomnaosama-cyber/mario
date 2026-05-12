#include "GameController.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QTimer>
#include <QPen>
#include <QCoreApplication>
#include <QDir>
#include <QApplication>
#include <QScreen>
#include "PiranhaPlant.h"



GameController::GameController(QGraphicsScene* s, int worldWidth, int screenH, int tileSize) 
<<<<<<< HEAD
    : scene(s), mario(nullptr), gamePlayer(nullptr), currentLevel(nullptr), finishItem(nullptr), 
      scoreText(nullptr), livesText(nullptr), levelText(nullptr), endMessageText(nullptr), tileSize(tileSize), 
      screenHeight(screenH), worldWidth(worldWidth), gameEnded(false), currentLevelNumber(1), 
      totalLevels(5), levelCompleted(false), bowser(nullptr), bossFightActive(false), 
      canDamageBowser(false), winZoneTimer(nullptr), winZoneItem(nullptr), marioHealthBar(nullptr),
      bowserHealthBar(nullptr) {

=======
    : scene(s), mario(nullptr), luigi(nullptr), gamePlayer(nullptr), currentLevel(nullptr), finishItem(nullptr), scoreText(nullptr), livesText(nullptr),
      endMessageText(nullptr), tileSize(tileSize), screenHeight(screenH), worldWidth(worldWidth), gameEnded(false) , isLevel3(true) {
    
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
    qDebug() << "GameController constructor started";
    qDebug() << "Screen height:" << screenHeight << "Tile size:" << tileSize;
    qDebug() << "Rows:" << ((screenHeight + tileSize - 1) / tileSize) << "Cols:" << (worldWidth / tileSize);
    
<<<<<<< HEAD
    mario = new MarioCharacter();
=======
    // Let Level create its own tiles based on screen dimensions
    int rows = (screenHeight + tileSize - 1) / tileSize;
    int cols = worldWidth / tileSize;
    
    // Create the level - it will initialize its own grid
    currentLevel = new Level(rows, cols);

    if (isLevel3) {
        currentLevel->createLevel3Tiles();
    } else {
        currentLevel->createTiles();
    }

        
    // Get spawn position from level (in pixels)
    auto spawn = currentLevel->getSpawn();

    qDebug() << "Spawn position (pixels):" << spawn.first << "," << spawn.second;

    mario = new MarioCharacter();

    int marioHeight = static_cast<int>(mario->boundingRect().height());
    mario->setPos(spawn.first, spawn.second - marioHeight);
    
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
    scene->addItem(mario);

    qDebug() << "Mario positioned at:" << mario->x() << "," << mario->y();
    
    //Creating Luigi ONLY in Level3
    if (isLevel3) {
    
    luigi = new LuigiCharacter();

    int luigiHeight = static_cast<int>(luigi->boundingRect().height());
    
    luigi->setPos(spawn.first + 100,spawn.second - luigiHeight); 

    scene->addItem(luigi);

    qDebug() << "Luigi added";
    
    } 
    
<<<<<<< HEAD
    gamePlayer = new Player(0, 0);
=======
    // Create player at spawn position
    gamePlayer = new Player(spawn.first, spawn.second);
    
    // Get enemies from level
    enemies = currentLevel->getenemy();

// =============================
// LEVEL 3: ADD PIRANHA PLANT
// =============================
if (isLevel3) {

    QPixmap plantImg;
    QString assetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        QDir::currentPath() + "/assets"
    };

    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            assetsPath = path;
            break;
        }
    }

    if (!assetsPath.isEmpty()) {
        plantImg.load(assetsPath + "/piranha.png");
    }

    // Example position (YOU CAN CHANGE LATER)
    int plantX = 25 * tileSize;
    int plantY = (screenHeight / tileSize - 2) * tileSize;

   // Create PiranhaPLant enemy object
   PiranhaPlant* plant =
    new PiranhaPlant(plantX / tileSize,
                     plantY / tileSize);

    // Add to enemies list
    enemies.push_back(plant);

    // Create graphics item
    QGraphicsPixmapItem* plantItem;

    if (!plantImg.isNull()) {

        QPixmap scaled =
            plantImg.scaled(
                tileSize,
                tileSize * 2,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );

        plantItem = scene->addPixmap(scaled);

    } else {

        QPixmap fallback(tileSize, tileSize * 2);
        fallback.fill(Qt::darkGreen);

        plantItem = scene->addPixmap(fallback);
    }

    plantItem->setPos(plantX, plantY);
    plantItem->setZValue(10);


    enemyGraphics.push_back(plantItem);


    qDebug() << "Piranha Plant added at:" << plantX << plantY;
}
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
    
    setupUI();
    
<<<<<<< HEAD
    loadLevel(currentLevelNumber);
=======
    // Load enemy image once for ALL enemies
    QPixmap enemyImage;
    QString assetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        "/home/yomna/mario/assets",
        QDir::currentPath() + "/assets",
        QDir::currentPath() + "/../assets"
    };
    
    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            assetsPath = path;
            break;
        }
    }
    
    if (!assetsPath.isEmpty()) {
        enemyImage.load(assetsPath + "/enemy.png");
        qDebug() << "Enemy image loaded from:" << assetsPath + "/enemy.png";
    } else {
        qDebug() << "No assets folder found, using fallback green square";
    }
    
    // Create graphics for each enemy (ALL use the same picture)
    for (Enemy* enemy : enemies) {
            if (!enemy)
        continue;

    // Skip Piranha Plant
    if (dynamic_cast<PiranhaPlant*>(enemy)) {
        continue;
    }

    int pixelX =
        static_cast<int>(
            enemy->getPreciseX() * tileSize
        );
        
    QGraphicsPixmapItem* enemyItem;
            
        if (!enemyImage.isNull()) {
            // ALL enemies use this same scaled image
            QPixmap scaled = enemyImage.scaled(tileSize - 5, tileSize - 5, 
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
            enemyItem = scene->addPixmap(scaled);
            } else {
                // Fallback - ALL enemies use this same green square
                QPixmap fallback(tileSize - 5, tileSize - 5);
                fallback.fill(Qt::green);
                enemyItem = scene->addPixmap(fallback);
            }
            
            int enemyHeight = static_cast<int>(enemyItem->boundingRect().height());
            int pixelY = enemy->getY() * tileSize - enemyHeight;
            enemyItem->setPos(pixelX, pixelY);
            enemyItem->setZValue(10);
            enemyGraphics.append(enemyItem);
            
            qDebug() << "Added enemy at tile:" << enemy->getX() << "," << enemy->getY() 
                     << "pixel:" << pixelX << "," << pixelY;
        }
    
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
    
    qDebug() << "GameController constructor finished";
}

GameController::~GameController() {
    delete gamePlayer;
    delete currentLevel;
    delete mario;
    delete luigi;
    
    for (QGraphicsPixmapItem* enemy : enemyGraphics) {
        delete enemy;
    }
    
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
}

void GameController::setupUI() {
    scoreText = scene->addText("Score: 0");
    scoreText->setDefaultTextColor(Qt::white);
    scoreText->setPos(10, 10);
    scoreText->setZValue(100);
    QFont font = scoreText->font();
    font.setPointSize(16);
    font.setBold(true);
    scoreText->setFont(font);
    
    livesText = scene->addText("Lives: 3");
    livesText->setDefaultTextColor(Qt::white);
    livesText->setPos(10, 40);
    livesText->setZValue(100);
    livesText->setFont(font);

    levelText = scene->addText("Level: 1");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setPos(10, 70);
    levelText->setZValue(100);
    levelText->setFont(font);

    updateUI();
}

void GameController::renderTiles() {
    if (isLevel3) {
    QString path;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        QDir::currentPath() + "/assets"
    };

    for (const QString& p : possiblePaths) {
        QDir dir(p);
        if (dir.exists()) {
            path = p;
            break;
        }
    }

    QPixmap bg(path + "/level3.png");
    scene->setBackgroundBrush(bg);
}

    int rows = (screenHeight + tileSize - 1) / tileSize;
    int cols = worldWidth / tileSize;
    QPixmap backgroundPixmap;
    QString backgroundImagePath = currentLevel->getBackgroundImagePath();
    QString assetsPath = findAssetsPath();
    
    if (!backgroundImagePath.isEmpty() && !assetsPath.isEmpty()) {
        backgroundPixmap.load(assetsPath + "/" + backgroundImagePath);
    }
    if (!backgroundPixmap.isNull()) {
        backgroundPixmap = backgroundPixmap.scaled(worldWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        scene->setBackgroundBrush(QBrush(backgroundPixmap));
    } else {
        QColor backgroundColor = currentLevel->getBackgroundColor();
        if (backgroundColor.isValid()) {
            scene->setBackgroundBrush(QBrush(backgroundColor));
        }
    }
    QPixmap finishImage;
    if (!assetsPath.isEmpty()) {
        finishImage.load(assetsPath + "/win.png");
    }
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Tile tile = currentLevel->getTileAt(i, j);
            if (tile.getType() != Tile::Empty) {
                if (tile.getType() == Tile::Flag && currentLevelNumber != 5) {
                    QPixmap flagPixmap = finishImage;
                    if (flagPixmap.isNull()) {
                        QPixmap fallback(tileSize, tileSize);
                        fallback.fill(Qt::red);
                        flagPixmap = fallback;
                    }
                    QPixmap scaled = flagPixmap.scaled(5.25*tileSize, 5.25*tileSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    finishItem = scene->addPixmap(scaled);
                    finishItem->setPos(j * tileSize, i * tileSize);
                    finishItem->setZValue(6);
                    continue;
                }

                QGraphicsRectItem* tileRect = scene->addRect(
                    j * tileSize, i * tileSize, tileSize, tileSize
                );
                
                QColor tileColor = currentLevel->getTileColor(tile.getType());
                tileRect->setBrush(tileColor);
                if (tile.getType() == Tile::Brick || tile.getType() == Tile::Mud || tile.getType() == Tile::Grass) {
                    tileRect->setPen(QPen(Qt::black, 1));
                } else {
                    tileRect->setPen(QPen(Qt::NoPen));
                }
                tileRect->setZValue(5);
            }
        }
    }
    qDebug() << "Tiles rendered!";
}

void GameController::cleanupCurrentLevel() {
    QList<QGraphicsItem*> itemsToRemove;
    for (QGraphicsItem* item : scene->items()) {
        if (item == scoreText || item == livesText || item == levelText || item == endMessageText || item == mario) {
            continue;
        }
        if (item == finishItem) {
            itemsToRemove.append(item);
            continue;
        }
        if (dynamic_cast<QGraphicsRectItem*>(item)) {
            itemsToRemove.append(item);
        }
    }

    for (QGraphicsItem* item : itemsToRemove) {
        scene->removeItem(item);
        delete item;
    }

    for (QGraphicsPixmapItem* enemy : enemyGraphics) {
        if (enemy) {
            scene->removeItem(enemy);
            delete enemy;
        }
    }
    enemyGraphics.clear();
    
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    
    delete currentLevel;
    currentLevel = nullptr;
    
    finishItem = nullptr;

    if (bowser) {
        scene->removeItem(bowser);
        delete bowser;
        bowser = nullptr;
    }
    
    if (marioHealthBar) {
        scene->removeItem(marioHealthBar);
        delete marioHealthBar;
        marioHealthBar = nullptr;
    }
    
    if (bowserHealthBar) {
        scene->removeItem(bowserHealthBar);
        delete bowserHealthBar;
        bowserHealthBar = nullptr;
    }
    
    if (winZoneItem) {
        scene->removeItem(winZoneItem);
        delete winZoneItem;
        winZoneItem = nullptr;
    }
    
    bossFightActive = false;
}

void GameController::loadLevel(int levelNumber) {
    qDebug() << "Loading level" << levelNumber;
    
    if (currentLevel) {
        cleanupCurrentLevel();
    }
    
    if (mario) {
        mario->setIsDead(false);
        mario->setVisible(true);
    }
    
    levelCompleted = false;
    
    int rows = (screenHeight + tileSize - 1) / tileSize;
    int cols = worldWidth / tileSize;
    
    switch(levelNumber) {
        case 1:
            currentLevel = new Level1(rows, cols);
            break;
        case 2:
            currentLevel = new Level2(rows, cols);
            break;
        case 3:
            currentLevel = new Level3(rows, cols);
            break;
        case 4:
            currentLevel = new Level4(rows, cols);
            break;
        case 5:
            currentLevel = new Level5(rows, cols);
            break;
        default:
            currentLevel = new Level1(rows, cols);
            break;
    }

    if (levelNumber == 5) {
        int bowserX = 60;
        // Use ceiling division — same formula Level5::createTiles uses for groundLevel
        int rows = (screenHeight + tileSize - 1) / tileSize;
        int groundLevel = rows - 2;
        // health=5 so exactly 5 jumps defeat Bowser
        bowser = new Bowser(bowserX, groundLevel, 50, 70, tileSize, 0.06f);
        scene->addItem(bowser);
        
        connect(bowser, &Bowser::healthChanged, this, [this](int current, int max) {
            if (bowserHealthBar) {
                bowserHealthBar->updateHealth(current, max);
            }
        });
        
        connect(bowser, &Bowser::died, this, [this]() {
            bossFightActive = false;
            if (bowserHealthBar) bowserHealthBar->hide();
            if (marioHealthBar) marioHealthBar->hide();
            showWinZone();
        });
                
        QString assetsPath = findAssetsPath();
        if (!assetsPath.isEmpty()) {
            QPixmap winImage;
            // Try win2.jpg first, then win2.png as fallback
            winImage.load(assetsPath + "/win2.jpg");
            if (winImage.isNull()) winImage.load(assetsPath + "/win2.png");
            if (!winImage.isNull()) {
                // Castle: large and visible — 6 tiles wide, 8 tiles tall
                int castleW = 6 * tileSize;   // 300px at tileSize=50
                int castleH = 8 * tileSize;   // 400px at tileSize=50
                // groundLevel * tileSize = top of ground tile row; castle bottom sits on it
                int castleX = 75 * tileSize;
                int castleY = groundLevel * tileSize - castleH;
                winZoneItem = scene->addPixmap(winImage.scaled(castleW, castleH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
                winZoneItem->setPos(castleX, castleY);
                winZoneItem->setZValue(6);
                winZoneItem->hide();  // shown only after Bowser is defeated
            }
        }
        
        // Health bars: positioned at fixed scene coords; repositioned each frame in updateOverlayPositions
        marioHealthBar = new HealthBar("Mario", 0, 10, 100);
        bowserHealthBar = new HealthBar("Bowser", 0, 50, 5);
        scene->addItem(marioHealthBar);
        scene->addItem(bowserHealthBar);
        marioHealthBar->updateHealth(100, 100);
        bowserHealthBar->updateHealth(5, 5);
        marioHealthBar->hide();
        bowserHealthBar->hide();
    }

    currentLevel->createTiles();
    
    auto spawn = currentLevel->getSpawn();
    int marioHeight = static_cast<int>(mario->boundingRect().height());
    mario->setPos(spawn.first, spawn.second - marioHeight);
    mario->respawn();
    
    gamePlayer->setPosition(spawn.first, spawn.second);
    gamePlayer->setHealth(100);
    
    enemies = currentLevel->getenemy();
    
    renderTiles();
    
    QPixmap enemyImage;
    QString assetsPath = findAssetsPath();
    if (!assetsPath.isEmpty()) {
        enemyImage.load(assetsPath + "/enemy.png");
    }
    if (enemyImage.isNull()) {
        QPixmap fallback(tileSize, tileSize);
        fallback.fill(Qt::red);
        enemyImage = fallback;
    }

    for (Enemy* enemy : enemies) {
        if (enemy) {
            QGraphicsPixmapItem* enemyPixmap = scene->addPixmap(enemyImage.scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            enemyPixmap->setZValue(15);
            qreal pixelX = enemy->getPreciseX() * tileSize;
            qreal pixelY = enemy->getY() * tileSize - enemyPixmap->boundingRect().height();
            enemyPixmap->setPos(pixelX, pixelY);
            enemyGraphics.push_back(enemyPixmap);
        } else {
            enemyGraphics.push_back(nullptr);
        }
    }
    
    qDebug() << "Level" << levelNumber << "loaded successfully";
}

void GameController::startNextLevel() {
    currentLevelNumber++;
    
    if (currentLevelNumber <= totalLevels) {
        qDebug() << "Starting next level:" << currentLevelNumber;
        levelCompleted = false;
        gamePlayer->setLives(3);
        loadLevel(currentLevelNumber);
        updateUI();
    } else {
        gameEnded = true;
        if (!endMessageText) {
            endMessageText = scene->addText("YOU BEAT THE GAME!");
            endMessageText->setDefaultTextColor(Qt::green);
            endMessageText->setFont(QFont("Arial", 48, QFont::Bold));
            endMessageText->setZValue(200);
        } else {
            endMessageText->setPlainText("YOU BEAT THE GAME!");
            endMessageText->setDefaultTextColor(Qt::green);
        }
        updateOverlayPositions();
        QTimer::singleShot(3000, []() {
            QCoreApplication::quit();
        });
    }
}

void GameController::startBossFight() {
    if (!bossFightActive && bowser && !bowser->getIsDead()) {
        bossFightActive = true;
        canDamageBowser = true;
        marioHealthBar->show();
        bowserHealthBar->show();
        qDebug() << "BOSS FIGHT STARTED! Jump on Bowser to damage him!";
    }
}

void GameController::updateBossFight() {
    if (bowser && !bowser->getIsDead()) {
        bowser->updateMovement();
    }
    
    if (marioHealthBar) {
        marioHealthBar->updateHealth(gamePlayer->getHealth(), 100);
    }
    updateOverlayPositions();
}

void GameController::checkBowserCollision() {
    if (!bowser || bowser->getIsDead() || !bossFightActive) return;
    
    QRectF marioRect = mario->sceneBoundingRect();
    QRectF bowserRect = bowser->sceneBoundingRect();
    
    if (!marioRect.intersects(bowserRect)) {
        bowserHitCooldown = false;
        return;
    }

    qreal marioFeet = mario->y() + 80;
    qreal bowserTop = bowserRect.top();
    // Mario jumped on Bowser if his feet are in the top 30% of Bowser's body and moving downward
    bool hitFromAbove = (marioFeet <= bowserTop + bowserRect.height() * 0.35)
                        && (mario->getVelocityY() > 0);

    if (hitFromAbove) {
        if (!bowserHitCooldown) {
            // Jump always damages Bowser regardless of which way he faces
            bowserHitCooldown = true;
            bowser->takeDamage(1);   // 5 hits total to defeat
            mario->setY(mario->y() - 25);
            mario->bounceUp();
            qDebug() << "Mario jumped on Bowser!";
            QTimer::singleShot(400, this, [this]() { bowserHitCooldown = false; });
        }
    } else {
        // Side collision: Mario takes damage, but with a cooldown to avoid instant-death
        if (!marioHitCooldown) {
            marioHitCooldown = true;
            gamePlayer->takeDamage(20);
            if (marioHealthBar) {
                marioHealthBar->updateHealth(gamePlayer->getHealth(), 100);
            }
            qDebug() << "Mario hit by Bowser! Health:" << gamePlayer->getHealth();

            QTimer::singleShot(800, this, [this]() { marioHitCooldown = false; });

            if (gamePlayer->getHealth() <= 0) {
                gamePlayer->loseLife();
                if (winZoneTimer) {
                    winZoneTimer->stop();
                    delete winZoneTimer;
                    winZoneTimer = nullptr;
                }
                mario->die();
                updateUI();

                if (gamePlayer->getLives() <= 0) {
                    gameEnded = true;
                    if (!endMessageText) {
                        endMessageText = scene->addText("GAME OVER");
                        endMessageText->setDefaultTextColor(Qt::red);
                        endMessageText->setFont(QFont("Arial", 48, QFont::Bold));
                        endMessageText->setZValue(200);
                    }
                    endMessageText->setVisible(true);
                    updateOverlayPositions();
                    QTimer::singleShot(3000, []() { QCoreApplication::quit(); });
                } else {
                    QTimer::singleShot(2000, this, [this]() {
                        auto spawn = currentLevel->getSpawn();
                        int marioHeight = static_cast<int>(mario->boundingRect().height());
                        mario->respawn();
                        mario->setIsDead(false);
                        mario->setPos(spawn.first, spawn.second - marioHeight);
                        gamePlayer->setPosition(spawn.first, spawn.second);
                        gamePlayer->setHealth(100);
                        marioHitCooldown = false;
                        if (marioHealthBar) {
                            marioHealthBar->updateHealth(100, 100);
                        }
                        updateUI();
                    });
                }
            }
        }
    }
}

void GameController::checkWinZone() {
    if (!winZoneItem || !winZoneItem->isVisible()) return;
    
    QRectF winRect = winZoneItem->sceneBoundingRect();
    QRectF marioRect = mario->sceneBoundingRect();
    
    if (marioRect.intersects(winRect)) {
        qreal marioCenterX = mario->x() + 40;
        qreal winCenterX = winRect.center().x();
        
        if (qAbs(marioCenterX - winCenterX) < 40) {
            if (!winZoneTimer) {
                winZoneTimer = new QTimer(this);
                winZoneTimer->setSingleShot(true);
                connect(winZoneTimer, &QTimer::timeout, this, [this]() {
                    if (!levelCompleted && !gameEnded && !mario->getIsDead()) {
                        levelCompleted = true;
                        gamePlayer->addScore(1000);
                        
                        if (!endMessageText) {
                            endMessageText = scene->addText("YOU BEAT THE GAME!");
                            endMessageText->setDefaultTextColor(Qt::green);
                            endMessageText->setFont(QFont("Arial", 48, QFont::Bold));
                            endMessageText->setZValue(200);
                        }
                        endMessageText->setVisible(true);
                        updateOverlayPositions();
                        
                        QTimer::singleShot(3000, []() {
                            QCoreApplication::quit();
                        });
                    }
                });
                winZoneTimer->start(1000);
                qDebug() << "Standing in win zone...";
            }
        } else {
            if (winZoneTimer) {
                winZoneTimer->stop();
                delete winZoneTimer;
                winZoneTimer = nullptr;
            }
        }
    } else {
        if (winZoneTimer) {
            winZoneTimer->stop();
            delete winZoneTimer;
            winZoneTimer = nullptr;
        }
    }
}

void GameController::showWinZone() {
    if (winZoneItem) {
        winZoneItem->show();
        qDebug() << "Win zone revealed! Stand in the middle to win!";
    }
}

QString GameController::findAssetsPath() {
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        "/home/yomna/mario/assets",
        QDir::currentPath() + "/assets",
        QDir::currentPath() + "/../assets"
    };
    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            return path;
        }
    }
    return "";
}

void GameController::updateGame() {
    if (!mario || mario->getIsDead() || gameEnded) return;

   mario->updatePhysics();

<<<<<<< HEAD
=======
    if (isLevel3) {
        qDebug() << "Level 3 active";
        
    }
    

    int previousMarioTop = static_cast<int>(mario->y());
   
    // Ground collision detection
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
    int marioHeight = 80;
    int marioWidth = 80;
    int marioBottom = static_cast<int>(mario->y()) + marioHeight;
    int marioLeft = static_cast<int>(mario->x());
    int marioRight = marioLeft + marioWidth - 1;

    int feetTileRow = (marioBottom + 5) / tileSize;
    int rows = (screenHeight + tileSize - 1) / tileSize;
    int cols = worldWidth / tileSize;

    bool standingOnSomething = false;
    
    for (int checkX : {marioLeft + 10, marioLeft + marioWidth/2, marioRight - 10}) {
        int col = checkX / tileSize;
        
        if (feetTileRow >= 0 && feetTileRow < rows && col >= 0 && col < cols) {
            Tile tile = currentLevel->getTileAt(feetTileRow, col);
            if (tile.getType() != Tile::Empty) {
                int tileTop = feetTileRow * tileSize;
                
                if (marioBottom >= tileTop - 10 && marioBottom <= tileTop + tileSize) {
                    mario->setY(tileTop - marioHeight);
                    mario->setIsOnGround(true);
                    standingOnSomething = true;
                    break;
                }
            }
        }
    }
    
    if (!standingOnSomething) {
        int exactTileRow = marioBottom / tileSize;
        for (int checkX : {marioLeft + 10, marioLeft + marioWidth/2, marioRight - 10}) {
            int col = checkX / tileSize;
            
            if (exactTileRow >= 0 && exactTileRow < rows && col >= 0 && col < cols) {
                Tile tile = currentLevel->getTileAt(exactTileRow, col);
                if (tile.getType() != Tile::Empty) {
                    int tileTop = exactTileRow * tileSize;
                    if (marioBottom >= tileTop - 10) {
                        mario->setY(tileTop - marioHeight);
                        mario->setIsOnGround(true);
                        standingOnSomething = true;
                        break;
                    }
                }
            }
        }
    }

  if (!standingOnSomething) {
        mario->setIsOnGround(false);
    }

<<<<<<< HEAD
    if (finishItem && currentLevelNumber != 5) {
=======
    checkCollisions();
    updateUI();
    
   

    // Allow Mario to stand on the top of the scaled finish image.
    if (finishItem) {
>>>>>>> 96a6d99e6202f04d821aa75f2423721c0ee77443
        QRectF finishRect = finishItem->sceneBoundingRect();
        qreal finishTop = finishRect.top();
        qreal marioBottomNow = mario->y() + marioHeight;
        qreal marioBottomPrev = marioBottomNow - 5.0;
        qreal marioLeftNow = mario->x();
        qreal marioRightNow = marioLeftNow + marioWidth;
        bool xOverlapsFinish = (marioRightNow > finishRect.left() + 8.0) && (marioLeftNow < finishRect.right() - 8.0);

        if (xOverlapsFinish && marioBottomPrev <= finishTop + 10.0 && marioBottomNow >= finishTop - 10.0) {
            mario->setY(finishTop - marioHeight);
            mario->setIsOnGround(true);
            standingOnSomething = true;
        }
    }

    int marioTop = static_cast<int>(mario->y());
    if (marioTop < previousMarioTop) {
        int topTileRow = marioTop / tileSize;
        int previousTopRow = previousMarioTop / tileSize;
        for (int checkX : {marioLeft + 10, marioLeft + marioWidth/2, marioRight - 10}) {
            int col = checkX / tileSize;
            if (topTileRow >= 0 && topTileRow < rows && col >= 0 && col < cols) {
                Tile tile = currentLevel->getTileAt(topTileRow, col);
                if (tile.getType() != Tile::Empty) {
                    if (previousTopRow > topTileRow) {
                        int tileBottom = (topTileRow + 1) * tileSize;
                        mario->setY(tileBottom);
                        mario->stopUpwardMotion();
                    }
                    break;
                }
            }
        }
    }

    if (currentLevelNumber == 5 && bowser && !bowser->getIsDead() && !bossFightActive) {
        if (mario->x() > 60 * tileSize) {
            startBossFight();
        }
    }
    
    if (bossFightActive && bowser && !bowser->getIsDead()) {
        updateBossFight();
    }
    
    checkBowserCollision();
    checkCollisions();
    checkWinZone();

    int marioFeetRow = (static_cast<int>(mario->y()) + marioHeight + 1) / tileSize;
    int marioCenterCol = (static_cast<int>(mario->x()) + marioWidth / 2) / tileSize;
    bool wonOnFinishImage = false;
    
    if (finishItem && currentLevelNumber != 5) {
        QRectF finishRect = finishItem->sceneBoundingRect();
        qreal marioBottomPos = mario->y() + marioHeight;
        qreal marioCenterX = mario->x() + marioWidth / 2.0;
        bool closeToFinishTop = qAbs(marioBottomPos - finishRect.top()) <= 12.0;
        bool centerOverFinish = marioCenterX >= finishRect.left() && marioCenterX <= finishRect.right();
        wonOnFinishImage = closeToFinishTop && centerOverFinish;
    }
    
    if (!levelCompleted && currentLevelNumber != 5 && (wonOnFinishImage || (marioFeetRow >= 0 && marioFeetRow < rows && marioCenterCol >= 0 && marioCenterCol < cols))) {
        Tile feetTile = currentLevel->getTileAt(marioFeetRow, marioCenterCol);
        if (wonOnFinishImage || feetTile.getType() == Tile::Flag) {
            levelCompleted = true;
            gamePlayer->addScore(500);
        
            if (!endMessageText) {
                endMessageText = scene->addText("LEVEL COMPLETE!");
                endMessageText->setDefaultTextColor(Qt::green);
                endMessageText->setFont(QFont("Arial", 36, QFont::Bold));
                endMessageText->setZValue(200);
            } else {
                endMessageText->setPlainText("LEVEL COMPLETE!");
                endMessageText->setDefaultTextColor(Qt::green);
            }
            endMessageText->setVisible(true);
            updateOverlayPositions();

            QTimer::singleShot(4000, this, [this]() {
                if (endMessageText && endMessageText->toPlainText() == "LEVEL COMPLETE!") {
                    endMessageText->setVisible(false);
                }
                startNextLevel();
            });
        }
    }

    if (mario->y() > screenHeight) {
        handleEnemyCollision(nullptr);
    }
    
    if (mario->x() < 0) mario->setX(0);
    if (mario->x() > worldWidth - marioWidth) 
    mario->setX(worldWidth - marioWidth);

    updateUI();
    mario->nextFrame();
}

bool GameController::checkCollisions() {
    for (size_t i = 0; i < enemies.size(); i++) {
        Enemy* enemy = enemies[i];

        if (!enemy) continue;
       
       //Piranha plant does NOT move like normal enemy
       PiranhaPlant* plant = dynamic_cast<PiranhaPlant*>(enemy);

        if (plant) {
            plant->update();   // only moves up/down
        } else {
            enemy->autoMove(); // normal enemies
        }
        
        if (enemy && i < static_cast<size_t>(enemyGraphics.size()) && enemyGraphics[i]) {
            
            int pixelX = static_cast<int>(enemy->getPreciseX() * tileSize);
            int enemyHeight = static_cast<int>(enemyGraphics[i]->boundingRect().height());
            int pixelY = enemy->getY() * tileSize - enemyHeight;
            enemyGraphics[i]->setPos(pixelX, pixelY);
            
            QRectF marioRect = mario->sceneBoundingRect();
            QRectF enemyRect = enemyGraphics[i]->sceneBoundingRect();
            
            if (marioRect.intersects(enemyRect)) {
                qreal marioFeet = mario->y() + 80;
                qreal enemyTop = enemyRect.top();
                qreal enemyMid = enemyTop + enemyRect.height() / 2.0;

                if (marioFeet <= enemyMid && marioFeet >= enemyTop - 10) {
                    delete enemyGraphics[i];
                    enemyGraphics[i] = nullptr;
                    delete enemies[i];
                    enemies[i] = nullptr;
                    gamePlayer->addScore(100);
                    updateUI();
                    qDebug() << "Enemy stomped! Score:" << gamePlayer->getScore();
                    
                    mario->setY(mario->y() - 20);
                } else {
                    handleEnemyCollision(enemy);
                    return true;
                }
            }
        }
    }
    return false;
}

void GameController::handleEnemyCollision(Enemy* /* enemy */) {
    if (!mario->getIsDead() && !gameEnded && !levelCompleted) {
        // Cancel any pending win-zone timer so dying never triggers "YOU BEAT THE GAME!"
        if (winZoneTimer) {
            winZoneTimer->stop();
            delete winZoneTimer;
            winZoneTimer = nullptr;
        }
        mario->die();
        gamePlayer->loseLife();
        gamePlayer->setHealth(100);
        updateUI();
        qDebug() << "Hit! Lives:" << gamePlayer->getLives();
        
        if (gamePlayer->getLives() <= 0) {
            qDebug() << "GAME OVER! Final Score:" << gamePlayer->getScore();
            gameEnded = true;

            if (!endMessageText) {
                endMessageText = scene->addText("GAME OVER");
                endMessageText->setZValue(200);
            } else {
                endMessageText->setPlainText("GAME OVER");
            }
            endMessageText->setDefaultTextColor(Qt::red);
            endMessageText->setFont(QFont("Arial", 48, QFont::Bold));
            endMessageText->setVisible(true);
            updateOverlayPositions();
            
            QTimer::singleShot(3000, []() {
                QCoreApplication::quit();
            });

        } else {
            QTimer::singleShot(2000, this, [this]() {
                if (!mario || !currentLevel) return;
                auto spawn = currentLevel->getSpawn();
                int marioHeight = static_cast<int>(mario->boundingRect().height());
                mario->respawn();
                mario->setIsDead(false);
                mario->setVisible(true);
                mario->setPos(spawn.first, spawn.second - marioHeight);
                gamePlayer->setPosition(spawn.first, spawn.second);
                gamePlayer->setHealth(100);
                updateUI();
                updateOverlayPositions();
            });
        }
    }
}

void GameController::updateUI() {
    if (scoreText) {
        scoreText->setPlainText("Score: " + QString::number(gamePlayer->getScore()));
    }
    if (livesText) {
        livesText->setPlainText("Lives: " + QString::number(gamePlayer->getLives()));
    }
    if (levelText) {
        levelText->setPlainText("Level: " + QString::number(currentLevelNumber));
    }
    updateOverlayPositions();
}

void GameController::updateOverlayPositions() {
    if (!mario || !scoreText || !livesText || !levelText) return;

    QScreen* screen = QApplication::primaryScreen();
    qreal viewportWidth = screen ? screen->geometry().width() : 1280;
    qreal viewportHeight = screen ? screen->geometry().height() : screenHeight;

    qreal cameraCenterX = mario->x() + 40;
    qreal topLeftX = cameraCenterX - viewportWidth / 2.0;
    qreal topLeftY = screenHeight / 2.0 - viewportHeight / 2.0;
    scoreText->setPos(topLeftX + 16, topLeftY + 12);
    livesText->setPos(topLeftX + 16, topLeftY + 40);
    if (levelText) {
        levelText->setPos(topLeftX + 16, topLeftY + 68);
    }

    // Stick health bars to top-right corner of viewport
    if (marioHealthBar && marioHealthBar->isVisible()) {
        qreal barX = topLeftX + viewportWidth - 320;
        marioHealthBar->setPos(barX - marioHealthBar->rect().x(), topLeftY + 10 - marioHealthBar->rect().y());
    }
    if (bowserHealthBar && bowserHealthBar->isVisible()) {
        qreal barX = topLeftX + viewportWidth - 320;
        bowserHealthBar->setPos(barX - bowserHealthBar->rect().x(), topLeftY + 50 - bowserHealthBar->rect().y());
    }

    if (endMessageText) {
        qreal centerY = screenHeight / 2.0;
        endMessageText->setPos(
            cameraCenterX - endMessageText->boundingRect().width() / 2.0,
            centerY - endMessageText->boundingRect().height() / 2.0
        );
    }
}