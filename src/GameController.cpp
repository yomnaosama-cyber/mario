#include "GameController.h"
#include "movingplatform.h"
#include "CrumblingTile.h"
#include "LuigiCharacter.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Level4.h"
#include "Level5.h"
#include "Tile.h"
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QTimer>
#include <QPen>
#include <QCoreApplication>
#include <QDir>
#include <QApplication>
#include <QScreen>

GameController::GameController(QGraphicsScene* s, int worldWidth, int screenH, int tileSize,
                               int levelNum, int startScore, int startLives)
    : scene(s),
    mario(nullptr),
    luigi(nullptr),
    gamePlayer(nullptr),
    currentLevel(nullptr),
    finishItem(nullptr),
    scoreText(nullptr),
    livesText(nullptr),
    levelText(nullptr),
    endMessageText(nullptr),
    tileSize(tileSize),
    screenHeight(screenH),
    worldWidth(worldWidth),
    gameEnded(false),
    levelCompleted(false),
    currentLevelNumber(qBound(1, levelNum, 5)),
    totalLevels(5),
    currentLevelDoor(nullptr),
    bowser(nullptr),
    marioHealthBar(nullptr),
    bowserHealthBar(nullptr),
    bossFightActive(false),
    canDamageBowser(false),
    winZoneTimer(nullptr),
    winZoneItem(nullptr) {

    qDebug() << "GameController constructor started";
    qDebug() << "Screen height:" << screenHeight << "Tile size:" << tileSize;
    qDebug() << "Rows:" << ((screenHeight + tileSize - 1) / tileSize) << "Cols:" << (worldWidth / tileSize);

    mario = new MarioCharacter();
    scene->addItem(mario);

    gamePlayer = new Player(0, 0);
    if (startScore > 0) {
        gamePlayer->addScore(startScore);
    }
    const int defaultLives = 3;
    for (int i = 0; i < defaultLives - startLives; ++i) {
        gamePlayer->loseLife();
    }

    setupUI();
    loadLevel(currentLevelNumber);

    qDebug() << "GameController constructor finished";
}

GameController::~GameController() {
    for (Coin* c : coins) {
        if (c && c->graphic) {
            scene->removeItem(c->graphic);
            delete c->graphic;
        }
        delete c;
    }
    coins.clear();
    for (Mushroom* m : mushrooms) {
        if (m && m->graphic) {
            scene->removeItem(m->graphic);
            delete m->graphic;
        }
        delete m;
    }
    mushrooms.clear();

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
    if (!currentLevel) {
        return;
    }

    const int rows = currentLevel->getRowCount();
    const int cols = currentLevel->getColCount();
    QString assetsPath = findAssetsPath();

    auto applyScaledBackground = [&](const QString& relativePath) {
        if (relativePath.isEmpty() || assetsPath.isEmpty()) {
            return false;
        }
        QPixmap bg(assetsPath + QStringLiteral("/") + relativePath);
        if (bg.isNull()) {
            return false;
        }
        scene->setBackgroundBrush(QBrush(bg.scaled(worldWidth, screenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
        return true;
    };

    // Level 1: classic overworld sky | 2: bonus / coins art | 3: water-plant stage | 4: tunnel | 5: castle / Bowser
    if (currentLevelNumber == 1) {
        if (!applyScaledBackground(QStringLiteral("sky1.jpg"))) {
            QColor c = currentLevel->getBackgroundColor();
            scene->setBackgroundBrush(c.isValid() ? QBrush(c) : QBrush(QColor(135, 206, 250)));
        }
    } else if (currentLevelNumber == 2) {
        if (!applyScaledBackground(currentLevel->getBackgroundImagePath())
            && !applyScaledBackground(QStringLiteral("sky1.jpg"))) {
            QColor c = currentLevel->getBackgroundColor();
            scene->setBackgroundBrush(c.isValid() ? QBrush(c) : QBrush(QColor(175, 220, 185)));
        }
    } else if (currentLevelNumber == 3) {
        if (!applyScaledBackground(QStringLiteral("level3.png"))) {
            QColor c = currentLevel->getBackgroundColor();
            scene->setBackgroundBrush(c.isValid() ? QBrush(c) : QBrush(QColor(70, 120, 160)));
        }
    } else if (currentLevelNumber == 4) {
        if (!applyScaledBackground(QStringLiteral("sky1.jpg"))) {
            QColor c = currentLevel->getBackgroundColor();
            scene->setBackgroundBrush(c.isValid() ? QBrush(c) : QBrush(QColor(48, 52, 68)));
        }
    } else if (currentLevelNumber == 5) {
        QString bgPath = currentLevel->getBackgroundImagePath();
        if (!applyScaledBackground(bgPath)) {
            scene->setBackgroundBrush(QBrush(QColor(35, 25, 40)));
        }
    } else {
        QString backgroundImagePath = currentLevel->getBackgroundImagePath();
        if (!applyScaledBackground(backgroundImagePath)) {
            QColor fallback = currentLevel->getBackgroundColor();
            scene->setBackgroundBrush(fallback.isValid() ? QBrush(fallback) : QBrush(QColor(135, 206, 250)));
        }
    }

    QPixmap finishImage;
    if (!assetsPath.isEmpty()) {
        finishImage.load(assetsPath + QStringLiteral("/win.png"));
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Tile* tile = currentLevel->getTileAt(i, j);
            if (!tile || tile->getType() == Tile::Empty) {
                continue;
            }

            if (tile->getType() == Tile::Flag && currentLevelNumber != 5) {
                QPixmap flagPixmap = finishImage;
                if (flagPixmap.isNull()) {
                    QPixmap fallback(tileSize, tileSize);
                    fallback.fill(Qt::red);
                    flagPixmap = fallback;
                }
                QPixmap scaled = flagPixmap.scaled(5.25 * tileSize, 5.25 * tileSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                finishItem = scene->addPixmap(scaled);
                finishItem->setPos(j * tileSize, i * tileSize);
                finishItem->setZValue(6);
                continue;
            }

            QColor tileColor = currentLevel->getTileColor(tile->getType());
            if (!tileColor.isValid()) {
                tileColor = QColor(139, 69, 19);
            }

            const bool sceneBackedTile = dynamic_cast<MovingPlatform*>(tile)
                || dynamic_cast<CrumblingTile*>(tile)
                || dynamic_cast<LockedDoor*>(tile)
                || dynamic_cast<SecretCoinTile*>(tile);

            if (sceneBackedTile) {
                QPixmap px(tileSize, tileSize);
                QString assetsPath = findAssetsPath();

                if(dynamic_cast<SecretCoinTile*>(tile)){
                    px.load(assetsPath + "/secret-Photoroom.png");
                }
                else if(dynamic_cast<LockedDoor*>(tile)){
                    px.load(assetsPath + "/door-Photoroom.png");
                }
                else if(dynamic_cast<CrumblingTile*>(tile) || dynamic_cast<MovingPlatform*>(tile)){
                    px.load(assetsPath + "/brick.png");
                }

                if(px.isNull()){
                    px = QPixmap(tileSize, tileSize);
                    px.fill(tileColor);
                }else{
                    px = px.scaled(tileSize, tileSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }

                if (dynamic_cast<LockedDoor*>(tile)) {
                    // --- A. Scale the Door Picture: One block wide, TWO blocks tall ---
                    QPixmap tallDoor = px.scaled(tileSize, tileSize * 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    tile->setPixmap(tallDoor);

                    tile->setPos(j * tileSize, (i - 1) * tileSize);

                } else {
                    // --- A. Scale standard items (keys, crumbling bricks): 1x1 size ---
                    QPixmap standardItem = px.scaled(tileSize, tileSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    tile->setPixmap(standardItem);

                    // --- B. Standard Position: Use original formula ---
                    tile->setPos(j * tileSize, i * tileSize);
                }


                tile->setPixmap(px);
                tile->setPos(j * tileSize, i * tileSize);
                tile->setZValue(5);
                scene->addItem(tile);
                continue;
            }

            QPen pen = (tile->getType() == Tile::Brick || tile->getType() == Tile::Mud || tile->getType() == Tile::Grass)
                ? QPen(Qt::black, 1) : QPen(Qt::NoPen);
            QGraphicsRectItem* groundRect = scene->addRect(
                j * tileSize, i * tileSize, tileSize, tileSize, pen, QBrush(tileColor));
            groundRect->setZValue(5);
            groundRect->setData(0, QStringLiteral("terrain"));
        }
    }
    qDebug() << "Tiles rendered!";
}

void GameController::cleanupCurrentLevel() {
    for (Coin* c : coins) {
        if (c && c->graphic) {
            scene->removeItem(c->graphic);
            delete c->graphic;
        }
        delete c;
    }
    coins.clear();
    for (Mushroom* m : mushrooms) {
        if (m && m->graphic) {
            scene->removeItem(m->graphic);
            delete m->graphic;
        }
        delete m;
    }
    mushrooms.clear();

    QList<QGraphicsItem*> itemsToRemove;
    for (QGraphicsItem* item : scene->items()) {
        if (item == scoreText || item == livesText || item == levelText || item == endMessageText || item == mario) {
            continue;
        }
        if (item == finishItem) {
            itemsToRemove.append(item);
            continue;
        }
        if (dynamic_cast<HealthBar*>(item)) {
            continue;
        }
        bool underHealthBar = false;
        for (QGraphicsItem* p = item->parentItem(); p; p = p->parentItem()) {
            if (dynamic_cast<HealthBar*>(p)) {
                underHealthBar = true;
                break;
            }
        }
        if (underHealthBar) {
            continue;
        }
        if (QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
            if (rectItem->data(0).toString() == QLatin1String("terrain")) {
                itemsToRemove.append(item);
            }
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

    if (luigi) {
        scene->removeItem(luigi);
        delete luigi;
        luigi = nullptr;
    }

    if (currentLevel) {
        const int r = currentLevel->getRowCount();
        const int c = currentLevel->getColCount();
        for (int ri = 0; ri < r; ++ri) {
            for (int ci = 0; ci < c; ++ci) {
                Tile* t = currentLevel->getTileAt(ri, ci);
                if (!t) {
                    continue;
                }
                if (dynamic_cast<MovingPlatform*>(t) || dynamic_cast<CrumblingTile*>(t)
                    || dynamic_cast<LockedDoor*>(t) || dynamic_cast<SecretCoinTile*>(t)) {
                    if (t->scene() == scene) {
                        scene->removeItem(t);
                    }
                }
            }
        }
    }

    delete currentLevel;
    currentLevel = nullptr;

    finishItem = nullptr;
    currentLevelDoor = nullptr;

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

    if (winZoneTimer) {
        winZoneTimer->stop();
        delete winZoneTimer;
        winZoneTimer = nullptr;
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
    currentLevelNumber = levelNumber;

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

    if (levelNumber == 4) {
        Level4* lev4 = dynamic_cast<Level4*>(currentLevel);
        if (lev4) {
            currentLevelDoor = lev4->getLockedDoor();
        }
    }

    auto spawn = currentLevel->getSpawn();
    int marioHeight = static_cast<int>(mario->boundingRect().height());
    mario->setPos(spawn.first, spawn.second - marioHeight);
    mario->respawn();

    gamePlayer->setPosition(spawn.first, spawn.second);
    gamePlayer->setHealth(100);

    if (levelNumber == 3) {
        luigi = new LuigiCharacter();
        QString ap = findAssetsPath();
        if (!ap.isEmpty()) {
            QPixmap lp(ap + "/luigi.png");
            if (!lp.isNull()) {
                luigi->setPixmap(lp.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
        int luigiHeight = static_cast<int>(luigi->boundingRect().height());
        luigi->setPos(spawn.first + tileSize * 2, spawn.second - luigiHeight);
        scene->addItem(luigi);
    }

    enemies = currentLevel->getenemy();

    renderTiles();

    QString assetsPath = findAssetsPath();
    QPixmap enemyImage;
    if (!assetsPath.isEmpty()) {
        enemyImage.load(assetsPath + "/enemy.png");
    }
    if (enemyImage.isNull()) {
        QPixmap fallback(tileSize, tileSize);
        fallback.fill(Qt::red);
        enemyImage = fallback;
    }

    QPixmap plantImage;
    if (!assetsPath.isEmpty()) {
        plantImage.load(assetsPath + "/piranha.png");
    }

    for (Enemy* enemy : enemies) {
        if (!enemy) {
            enemyGraphics.push_back(nullptr);
            continue;
        }

        PiranhaPlant* plant = dynamic_cast<PiranhaPlant*>(enemy);
        if (plant) {
            QGraphicsPixmapItem* plantItem;
            if (!plantImage.isNull()) {
                QPixmap scaled = plantImage.scaled(tileSize, tileSize * 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                plantItem = scene->addPixmap(scaled);
            } else {
                QPixmap fb(tileSize, tileSize * 2);
                fb.fill(Qt::darkGreen);
                plantItem = scene->addPixmap(fb);
            }
            plantItem->setZValue(16);
            int ph = static_cast<int>(plantItem->boundingRect().height());
            plantItem->setPos(plant->getPreciseX() * tileSize, plant->getY() * tileSize - ph + plant->getBobPixelOffset());
            enemyGraphics.push_back(plantItem);
        } else {
            QGraphicsPixmapItem* enemyPixmap = scene->addPixmap(
                enemyImage.scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            enemyPixmap->setZValue(15);
            qreal pixelX = enemy->getPreciseX() * tileSize;
            qreal pixelY = enemy->getY() * tileSize - enemyPixmap->boundingRect().height();
            enemyPixmap->setPos(pixelX, pixelY);
            enemyGraphics.push_back(enemyPixmap);
        }
    }

    if (levelNumber == 2) {
        spawnCoins();
        spawnMushrooms();
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

    int previousMarioTop = static_cast<int>(mario->y());
    mario->updatePhysics();

    if (currentLevelNumber == 4 && currentLevel) {
        const int pr = currentLevel->getRowCount();
        const int pc = currentLevel->getColCount();
        for (int ri = 0; ri < pr; ++ri) {
            for (int ci = 0; ci < pc; ++ci) {
                Tile* tt = currentLevel->getTileAt(ri, ci);
                if (auto* mp = dynamic_cast<MovingPlatform*>(tt)) {
                    mp->update();
                }
            }
        }
    }

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
            Tile* tile = currentLevel->getTileAt(feetTileRow, col);
            if (tile && tile->getType() != Tile::Empty) {
                int tileTop = feetTileRow * tileSize;

                CrumblingTile* ct1 = dynamic_cast<CrumblingTile*>(tile);
                if (ct1 && ct1->getState() == CrumblingTile::Disappear) {
                    continue;
                }

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
                Tile* tile = currentLevel->getTileAt(exactTileRow, col);
                if (tile && tile->getType() != Tile::Empty) {
                    int tileTop = exactTileRow * tileSize;
                    CrumblingTile* ct2 = dynamic_cast<CrumblingTile*>(tile);
                    if (ct2 && ct2->getState() == CrumblingTile::Disappear) {
                        continue;
                    }
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
    if (!standingOnSomething) {
        mario->setIsOnGround(false);
    }
    // Luigi physics (Level 3 only)
    if (luigi && currentLevelNumber == 3 && currentLevel) {
        luigi->updatePhysics();


        int luigiHeight = 80, luigiWidth = 80;
        int luigiBottom = static_cast<int>(luigi->y()) + luigiHeight;
        int luigiLeft   = static_cast<int>(luigi->x());
        int luigiRight  = luigiLeft + luigiWidth - 1;
        int lfeetRow    = (luigiBottom + 5) / tileSize;
        bool luigiOnGround = false;
        for (int checkX : {luigiLeft + 10, luigiLeft + luigiWidth/2, luigiRight - 10}) {
            int col = checkX / tileSize;
            if (lfeetRow >= 0 && lfeetRow < rows && col >= 0 && col < cols) {
                Tile* tile = currentLevel->getTileAt(lfeetRow, col);
                if (tile && tile->getType() != Tile::Empty) {
                    int tileTop = lfeetRow * tileSize;
                    if (luigiBottom >= tileTop - 10 && luigiBottom <= tileTop + tileSize) {
                        luigi->setY(tileTop - luigiHeight);
                        luigi->setIsOnGround(true);
                        luigiOnGround = true;
                        break;
                    }
                }
            }
        }
        if (!luigiOnGround) {
            int lexactRow = luigiBottom / tileSize;
            for (int checkX : {luigiLeft + 10, luigiLeft + luigiWidth/2, luigiRight - 10}) {
                int col = checkX / tileSize;
                if (lexactRow >= 0 && lexactRow < rows && col >= 0 && col < cols) {
                    Tile* tile = currentLevel->getTileAt(lexactRow, col);
                    if (tile && tile->getType() != Tile::Empty) {
                        int tileTop = lexactRow * tileSize;
                        if (luigiBottom >= tileTop - 10) {
                            luigi->setY(tileTop - luigiHeight);
                            luigi->setIsOnGround(true);
                            luigiOnGround = true;
                            break;
                        }
                    }
                }
            }
        }
        if (!luigiOnGround) luigi->setIsOnGround(false);
        // Ceiling check
        int luigiTop = static_cast<int>(luigi->y());
        int ltopRow  = luigiTop / tileSize;
        for (int checkX : {luigiLeft + 10, luigiLeft + luigiWidth/2, luigiRight - 10}) {
            int col = checkX / tileSize;
            if (ltopRow >= 0 && ltopRow < rows && col >= 0 && col < cols) {
                Tile* tile = currentLevel->getTileAt(ltopRow, col);
                if (tile && tile->getType() != Tile::Empty) {
                    luigi->setY((ltopRow + 1) * tileSize);
                    luigi->stopUpwardMotion();
                    break;
                }
            }
        }
        // Boundary clamp + fall-off respawn
        if (luigi->x() < 0) luigi->setX(0);
        if (luigi->x() > worldWidth - luigiWidth) luigi->setX(worldWidth - luigiWidth);
        if (luigi->y() > screenHeight) {
            //auto spawn = currentLevel->getSpawnPoint();
            //luigi->setPos(spawn.first + tileSize * 2, spawn.second - luigiHeight);
            luigi->setIsOnGround(true);
        }
    }


    if (finishItem && currentLevelNumber != 5) {
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
                Tile* tile = currentLevel->getTileAt(topTileRow, col);
                if (tile && tile->getType() != Tile::Empty) {
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


    QList<QGraphicsItem*> collidingItems = mario->collidingItems();

    for(QGraphicsItem* item: collidingItems){
        MovingPlatform* platform = dynamic_cast<MovingPlatform*>(item);
        if (platform) {
            QRectF plat = platform->sceneBoundingRect();
            qreal feet = mario->y() + marioHeight;
            if (feet <= plat.top() + 18.0) {
                mario->setX(mario->x() + platform->getDeltaX());
                mario->setY(mario->y() + platform->getDeltaY());
                if (mario->y() + marioHeight > plat.top()) {
                    mario->setY(plat.top() - marioHeight);
                }
                mario->setIsOnGround(true);
                standingOnSomething = true;
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
    if (currentLevelNumber == 4) {
        checkCollisions4();
    }
    checkCoinCollisions();
    checkMushroomCollisions();
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
        Tile* feetTile = currentLevel->getTileAt(marioFeetRow, marioCenterCol);
        if (wonOnFinishImage || (feetTile && feetTile->getType() == Tile::Flag)) {
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
    if (mario->x() > worldWidth - marioWidth) mario->setX(worldWidth - marioWidth);

    updateUI();
    mario->nextFrame();
}

void GameController::checkCollisions4() {
    if (!currentLevel || !currentLevelDoor) {
        return;
    }

    const int marioWidth = 80;
    const int marioHeight = 80;
    const int centerX = static_cast<int>(mario->x() + marioWidth / 2);
    const int feetY = static_cast<int>(mario->y() + marioHeight);

    const int col = centerX / tileSize;
    const int row = feetY / tileSize;

    if (row < 0 || row >= currentLevel->getRowCount() || col < 0 || col >= currentLevel->getColCount()) {
        return;
    }

    Tile* currenttile = currentLevel->getTileAt(row, col);
    if (!currenttile) {
        return;
    }

    SecretCoinTile* secretCoin = dynamic_cast<SecretCoinTile*>(currenttile);
    if (secretCoin) {
        currentLevelDoor->addKey();

        gamePlayer->addScore(200);
        updateUI();

        if (currenttile->scene()) {
            scene->removeItem(currenttile);
        }
        delete currentLevel->grid[row][col];
        currentLevel->grid[row][col] = new Tile(static_cast<long long>(col), static_cast<long long>(row), Tile::Empty);

        if (currentLevelDoor->getkeyscollected() >= 5) {
            const int doorRow = currentLevelDoor->getrow();
            const int doorCol = currentLevelDoor->getcol();
            if (doorRow >= 0 && doorRow < currentLevel->getRowCount()
                && doorCol >= 0 && doorCol < currentLevel->getColCount()) {
                Tile* doorTile = currentLevel->grid[doorRow][doorCol];
                if (doorTile && doorTile->scene()) {
                    scene->removeItem(doorTile);
                }
                delete currentLevel->grid[doorRow][doorCol];
                currentLevel->grid[doorRow][doorCol] = new Tile(static_cast<long long>(doorCol), static_cast<long long>(doorRow), Tile::Empty);
            }
            currentLevelDoor = nullptr;
        }
        return;
    }

    if (auto* crumb = dynamic_cast<CrumblingTile*>(currenttile)) {
        if (crumb->getState() == CrumblingTile::Normal) {
            crumb->trigger();
        }
        else if(crumb->getState() == CrumblingTile::Disappear){
            if(currenttile->scene()){
                scene->removeItem(crumb);
            }
            currentLevel->grid[row][col] =
                new Tile(static_cast<long long>(col), static_cast<long long>(row), Tile::Empty);

            mario->setIsOnGround(false);
        }
    }
}

bool GameController::checkCollisions() {
    for (size_t i = 0; i < enemies.size(); i++) {
        Enemy* enemy = enemies[i];

        if (enemy && i < static_cast<size_t>(enemyGraphics.size()) && enemyGraphics[i]) {
            PiranhaPlant* plant = dynamic_cast<PiranhaPlant*>(enemy);
            if (plant) {
                plant->update();
            } else {
                enemy->autoMove();
            }

            int pixelX = static_cast<int>(enemy->getPreciseX() * tileSize);
            int enemyHeight = static_cast<int>(enemyGraphics[i]->boundingRect().height());
            int pixelY = enemy->getY() * tileSize - enemyHeight;
            if (plant) {
                pixelY += static_cast<int>(plant->getBobPixelOffset());
            }
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
            if (luigi && enemyGraphics[i]) {
                QRectF luigiRect = luigi->sceneBoundingRect();

                if (luigiRect.intersects(enemyRect)) {
                    qreal luigiFeet = luigi->y() + 80;
                    qreal enemyTop = enemyRect.top();
                    qreal enemyMid = enemyTop + enemyRect.height() / 2.0;

                    if (luigiFeet <= enemyMid && luigiFeet >= enemyTop - 10) {
                        delete enemyGraphics[i];
                        enemyGraphics[i] = nullptr;
                        delete enemies[i];
                        enemies[i] = nullptr;
                        gamePlayer->addScore(100);
                        updateUI();
                        qDebug() << "Enemy stomped! Score:" << gamePlayer->getScore();

                        luigi->setY(luigi->y() - 20);
                    } else {

                        handleEnemyCollision(enemy);
                        return true;
                    }
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
void GameController::spawnCoins() {
    if (currentLevelNumber != 2) {
        return;
    }

    int rows = (screenHeight + tileSize - 1) / tileSize;
    int groundLevel = rows - 2;

    const QList<QPair<int, int>> coinPositions = {
        {14, groundLevel - 4}, {15, groundLevel - 4},
        {26, groundLevel - 5}, {27, groundLevel - 5},
        {42, groundLevel - 3}, {43, groundLevel - 3}, {44, groundLevel - 3},
        {56, groundLevel - 6}, {57, groundLevel - 6},
        {70, groundLevel - 2}, {71, groundLevel - 2},
        {5, groundLevel - 2}, {6, groundLevel - 2},
    };

    for (auto& pos : coinPositions) {
        int cx = pos.first;
        int cy = pos.second;
        if (cy < 0) {
            cy = 0;
        }
        Coin* coin = new Coin(cx, cy);

        int px = cx * tileSize + tileSize / 2 - 8;
        int py = cy * tileSize + 2;
        QGraphicsEllipseItem* circle = scene->addEllipse(px, py, 16, 16,
                                                         QPen(QColor(180, 130, 0), 2), QBrush(QColor(255, 215, 0)));
        circle->setZValue(8);

        QGraphicsTextItem* label = scene->addText(QStringLiteral("C"));
        label->setDefaultTextColor(QColor(150, 100, 0));
        QFont f = label->font();
        f.setPointSize(8);
        f.setBold(true);
        label->setFont(f);
        label->setPos(px + 2, py - 1);
        label->setZValue(9);
        label->setParentItem(circle);

        coin->graphic = circle;
        coins.push_back(coin);
    }
}

void GameController::spawnMushrooms() {
    if (currentLevelNumber != 2) {
        return;
    }

    int rows = (screenHeight + tileSize - 1) / tileSize;
    int groundLevel = rows - 2;

    const QList<QPair<int, int>> shroomPositions = {
        {22, groundLevel - 3},
        {48, groundLevel - 3},
    };

    for (auto& pos : shroomPositions) {
        int mx = pos.first;
        int my = pos.second;
        if (my < 0) {
            my = 0;
        }
        Mushroom* shroom = new Mushroom(mx, my);

        int px = mx * tileSize + 4;
        int py = my * tileSize;
        int sz = tileSize - 8;

        QGraphicsRectItem* stem = scene->addRect(px + sz / 3, py + sz / 2, sz / 3, sz / 2,
                                                 QPen(Qt::NoPen), QBrush(QColor(240, 220, 180)));
        stem->setZValue(8);

        QGraphicsEllipseItem* cap = scene->addEllipse(px, py, sz, sz * 0.7,
                                                      QPen(QColor(150, 0, 0), 2), QBrush(QColor(220, 30, 30)));
        cap->setZValue(9);

        QGraphicsEllipseItem* spot1 = scene->addEllipse(px + sz * 0.2, py + sz * 0.1, sz * 0.15, sz * 0.15,
                                                        QPen(Qt::NoPen), QBrush(Qt::white));
        spot1->setZValue(10);
        QGraphicsEllipseItem* spot2 = scene->addEllipse(px + sz * 0.55, py + sz * 0.08, sz * 0.12, sz * 0.12,
                                                        QPen(Qt::NoPen), QBrush(Qt::white));
        spot2->setZValue(10);

        stem->setParentItem(cap);
        spot1->setParentItem(cap);
        spot2->setParentItem(cap);

        shroom->graphic = cap;
        mushrooms.push_back(shroom);
    }
}
void GameController::checkCoinCollisions() {
    QRectF marioRect = mario->sceneBoundingRect();


    for (Coin* coin : coins) {
        if (coin->collected || !coin->graphic) continue;
        QRectF coinRect = coin->graphic->sceneBoundingRect();
        if (marioRect.intersects(coinRect)) {
            coin->collected = true;
            coin->graphic->setVisible(false);
            // also hide the "C" label child
            for (QGraphicsItem* child : coin->graphic->childItems())
                child->setVisible(false);
            gamePlayer->addScore(50);
            updateUI();
            qDebug() << "Coin collected! Score:" << gamePlayer->getScore();
        }
    }
}

// ─── Mushroom collision ───────────────────────────────────────────────────────
void GameController::checkMushroomCollisions() {
    QRectF marioRect = mario->sceneBoundingRect();
    for (Mushroom* shroom : mushrooms) {
        if (shroom->collected || !shroom->graphic) continue;
        QRectF shroomRect = shroom->graphic->sceneBoundingRect();
        if (marioRect.intersects(shroomRect)) {
            shroom->collected = true;
            shroom->graphic->setVisible(false);
            mario->growBig();
            gamePlayer->addScore(200);
            updateUI();
            qDebug() << "Mushroom collected! Mario is big.";
        }
    }
}

/*void GameController::skipToNextLevel(){
    if(levelnum >= 5){
        qDebug()<<"Already at final level";
        return;
    }

    int nextLevel = levelnum+1;

    scene->clear();
    movingPlatformGraphics.clear();
    crumblingGraphics.clear();
    coins.clear();
    mushrooms.clear();

    this->levelnum = nextLevel;
    loadLevel(levelnum);


}*/
