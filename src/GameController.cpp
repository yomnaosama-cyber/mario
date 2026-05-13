#include "GameController.h"
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
    : scene(s), mario(nullptr), luigi(nullptr), gamePlayer(nullptr), currentLevel(nullptr), finishItem(nullptr), scoreText(nullptr), livesText(nullptr),
      endMessageText(nullptr), tileSize(tileSize), screenHeight(screenH), worldWidth(worldWidth), gameEnded(false) , isLevel3(true) {
    
    
    luigiMoveLeft = false; 
    luigiMoveRight = false; 
    luigiJump = false; 

    scene->installEventFilter(this);
    scene->setFocus();
    scene->setFocusItem(nullptr);


    qDebug() << "GameController constructor started";
    qDebug() << "Screen height:" << screenHeight << "Tile size:" << tileSize;
    qDebug() << "Rows:" << ((screenHeight + tileSize - 1) / tileSize) << "Cols:" << (worldWidth / tileSize);
    
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
    
    // Create player at spawn position
    gamePlayer = new Player(spawn.first, spawn.second);

    setupUI();
    renderTiles();
    
    // Get enemies from level
    enemies = currentLevel->getenemy();

//// =============================
// LEVEL 3: ADD PIRANHA PLANT
// =============================
if (isLevel3) {

    QPixmap plantImg;
    QString plantAssetsPath;    
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        QDir::currentPath() + "/assets"
    };

    for (const QString& path : possiblePaths) {
    QDir dir(path);

    if (dir.exists()) {
        plantAssetsPath = path;
        break;
    }
}

    if (!plantAssetsPath.isEmpty()) {
        plantImg.load(plantAssetsPath + "/piranha.png");
    }

    int plantX = 25 * tileSize;
    int plantY = (screenHeight / tileSize - 2) * tileSize;

    // creating LOGIC enemy FIRST
    PiranhaPlant* plant =
        new PiranhaPlant(plantX / tileSize, plantY / tileSize);

    plant->setPixelPos(plantX, plantY);

    // Add to enemies 
    enemies.push_back(plant);

    // creating GRAPHICS item
    QGraphicsPixmapItem* plantItem = nullptr;

    if (!plantImg.isNull()) {
        QPixmap scaled = plantImg.scaled(
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
    plantItem->setZValue(20);

    // push graphics AFTER creation 
    enemyGraphics.push_back(plantItem);

    qDebug() << "Piranha Plant added at:" << plantX << plantY;
}
    
    // Load enemy image once for ALL enemies
    QPixmap enemyImage;
    QString enemyAssetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        QDir::currentPath() + "/assets"
    };
    

    for (const QString& path : possiblePaths) {

        QDir dir(path);

        if (dir.exists()) {
            enemyAssetsPath = path;
            break;
        }
    }

    if (!enemyAssetsPath.isEmpty()) {

        enemyImage.load(enemyAssetsPath + "/enemy.png");

        qDebug() << "Enemy image loaded from:"
                << enemyAssetsPath + "/enemy.png";

    } else {

        qDebug() << "No assets folder found, using fallback green square";
    }

    // Create graphics for each enemy (ALL use the same picture)
    for (Enemy* enemy : enemies) {
            if (!enemy)
        continue;


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
    
    // Clean up enemies
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
    QPixmap finishImage;
    QString assetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
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
        finishImage.load(assetsPath + "/win.png");
    }
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Tile tile = currentLevel->getTileAt(i, j);
            if (tile.getType() != Tile::Empty) {
                if (tile.getType() == Tile::Flag) {
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
                
                switch(tile.getType()) {
                    case Tile::Brick:
                        tileRect->setBrush(QColor(160, 82, 45));
                        tileRect->setPen(QPen(Qt::black, 1));
                        break;
                    case Tile::Mud:
                        tileRect->setBrush(QColor(101, 67, 33));
                        tileRect->setPen(QPen(Qt::black, 1));
                        break;
                    case Tile::Grass:
                        tileRect->setBrush(QColor(34, 139, 34));
                        tileRect->setPen(QPen(Qt::black, 1));
                        break;
                    default:
                        tileRect->setBrush(QColor(139, 69, 19));
                        break;
                }
                tileRect->setZValue(5);
            }
        }
    }
    qDebug() << "Tiles rendered!";
}

void GameController::updateGame() {
    if (!mario || mario->getIsDead() || gameEnded) return;

   mario->updatePhysics();

    if (luigi) {

        // Luigi mov 
        if (luigiMoveLeft && !luigiMoveRight)
            luigi->setX(luigi->x() - 3);
        else if (luigiMoveRight && !luigiMoveLeft)
            luigi->setX(luigi->x() + 3);
        
        // JUMP LOGIC
        if (luigiJump && luigi->getIsOnGround()) {
            luigi->setVelocityY(-12);   // jump strength
            luigi->setIsOnGround(false);
}

        //  Luigi physics
        luigi->updatePhysics();
    }

    if (isLevel3) {
        qDebug() << "Level 3 active";
        
    }
    

    int previousMarioTop = static_cast<int>(mario->y());
   
    // Ground collision detection
    int marioHeight = 80;
    int marioWidth = 80;
    int marioBottom = static_cast<int>(mario->y()) + marioHeight;
    int marioLeft = static_cast<int>(mario->x());
    int marioRight = marioLeft + marioWidth - 1;

    // Check the tile row just below Mario's feet
    int feetTileRow = (marioBottom + 5) / tileSize;  // Check slightly below feet
    int rows = (screenHeight + tileSize - 1) / tileSize;
    int cols = worldWidth / tileSize;

    bool standingOnSomething = false;
    
    // Check multiple points under Mario
    for (int checkX : {marioLeft + 10, marioLeft + marioWidth/2, marioRight - 10}) {
        int col = checkX / tileSize;
        
        if (feetTileRow >= 0 && feetTileRow < rows && col >= 0 && col < cols) {
            Tile tile = currentLevel->getTileAt(feetTileRow, col);
            if (tile.getType() != Tile::Empty) {
                int tileTop = feetTileRow * tileSize;
                
                // If Mario's feet are at or below the tile top, snap him on top
                if (marioBottom >= tileTop - 10 && marioBottom <= tileTop + tileSize) {
                    mario->setY(tileTop - marioHeight);
                    mario->setIsOnGround(true);
                    standingOnSomething = true;
                    break;
                }
            }
        }
    }
    
    // If not standing on a tile, check the row exactly at feet level
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

    // Allow Mario to stand on the top of the scaled finish image.
    if (finishItem) {
        QRectF finishRect = finishItem->sceneBoundingRect();
        qreal finishTop = finishRect.top();
        qreal marioBottomNow = mario->y() + marioHeight;
        qreal marioBottomPrev = marioBottomNow - 5.0;  // small tolerance for frame-to-frame movement
        qreal marioLeftNow = mario->x();
        qreal marioRightNow = marioLeftNow + marioWidth;
        bool xOverlapsFinish = (marioRightNow > finishRect.left() + 8.0) && (marioLeftNow < finishRect.right() - 8.0);

        if (xOverlapsFinish && marioBottomPrev <= finishTop + 10.0 && marioBottomNow >= finishTop - 10.0) {
            mario->setY(finishTop - marioHeight);
            mario->setIsOnGround(true);
            standingOnSomething = true;
        }
    }

    // Ceiling collision: prevent Mario from passing upward through platforms.
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

    // Win check: Mario stands on the finish image top.
    int marioFeetRow = (static_cast<int>(mario->y()) + marioHeight + 1) / tileSize;
    int marioCenterCol = (static_cast<int>(mario->x()) + marioWidth / 2) / tileSize;
    bool wonOnFinishImage = false;
    if (finishItem) {
        QRectF finishRect = finishItem->sceneBoundingRect();
        qreal marioBottom = mario->y() + marioHeight;
        qreal marioCenterX = mario->x() + marioWidth / 2.0;
        bool closeToFinishTop = qAbs(marioBottom - finishRect.top()) <= 12.0;
        bool centerOverFinish = marioCenterX >= finishRect.left() && marioCenterX <= finishRect.right();
        wonOnFinishImage = closeToFinishTop && centerOverFinish;
    }
    if (wonOnFinishImage || (marioFeetRow >= 0 && marioFeetRow < rows && marioCenterCol >= 0 && marioCenterCol < cols)) {
        Tile feetTile = currentLevel->getTileAt(marioFeetRow, marioCenterCol);
        if (wonOnFinishImage || feetTile.getType() == Tile::Flag) {
            gameEnded = true;
            if (!endMessageText) {
                endMessageText = scene->addText("YOU WON!");
                endMessageText->setDefaultTextColor(Qt::green);
                endMessageText->setFont(QFont("Arial", 48, QFont::Bold));
                endMessageText->setZValue(200);
            } else {
                endMessageText->setPlainText("YOU WON!");
                endMessageText->setDefaultTextColor(Qt::green);
            }
            updateOverlayPositions();
            QTimer::singleShot(3000, []() {
                QCoreApplication::quit();
            });
        }
    }

    // Check if fell off world (below bottom row)
    if (mario->y() > screenHeight) {
        handleEnemyCollision(nullptr);
    }
    
    // Boundary checks (keep Mario in world)
    if (mario->x() < 0) mario->setX(0);
    if (mario->x() > worldWidth - marioWidth) 
    mario->setX(worldWidth - marioWidth);

    updateUI();
    checkCollisions();
    mario->nextFrame();
}

bool GameController::checkCollisions() {
    for (size_t i = 0; i < enemies.size(); i++) {
        
        Enemy* enemy = enemies[i];
        if (!enemy) continue;
       
       //Piranha plant mov
       PiranhaPlant* plant = dynamic_cast<PiranhaPlant*>(enemy);

        if (plant) {
            plant->update();   // ONLY plant moves vertically
        } else {
            enemy->autoMove();
        }
      
        if (i >= enemyGrpahics.size() || !enemyGrpahics[i]) continue; 
            
            // Convert tile coordinates to pixel coordinates for graphics
            int pixelX = enemy->getX() * tileSize;
            int pixelY = enemy->getY() * tileSize;
            enemyGraphics[i]->setPos(pixelX, pixelY);            
                      
            QRectF enemyRect = enemyGraphics[i]->sceneBoundingRect();
            QRectF marioRect = mario->sceneBoundingRect();
            
            bool marioHit = marioRect.intersects(enemyRect);

            QRectF luigiRect;
            bool luigiHit = false;
            if (luigi) {
                luigiRect = luigi->sceneBoundingRect();
                luigiHit = luigiRect.intersects(enemyRect);
            }


            // MARIO collision 
            if (marioHit) {

                qreal marioFeet = mario->y() + 80;
                qreal enemyTop = enemyRect.top();
                qreal enemyMid = enemyTop + enemyRect.height() / 2.0;
            
                if (marioFeet <= enemyMid && marioFeet >= enemyTop - 10) {
                    // stomp enemy
                    delete enemyGraphics[i];
                    enemyGraphics.erase(enemyGraphics.begin() + i);
                    delete enemies[i];
                    enemies.erase(enemies.begin() + i);
                    i--;
            
                    gamePlayer->addScore(100);
                    updateUI();
            
                    mario->setY(mario->y() - 20);
                } 
                else {
                    handleEnemyCollision(enemy);
                    return true;
                }
            }

            // LUIGI collision
            if (luigiHit) {

                qDebug() << "Luigi hit enemy! Score:" << gamePlayer->getScore();

                // simple safe response - no life system shared
                auto spawn = currentLevel->getSpawn();
                int luigiHeight = static_cast<int>(luigi->boundingRect().height());
                luigi->setPos(spawn.first + 100, spawn.second - luigiHeight);
                luigi->setVelocityY(0);
                luigi->setIsOnGround(false);
        }
    }

    return false;
}

void GameController::handleEnemyCollision(Enemy* /* enemy */) {
    if (!mario->getIsDead() && !gameEnded) {
        mario->die();
        gamePlayer->loseLife();
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
            updateOverlayPositions();
            
            // Exit the application after 3 seconds
            QTimer::singleShot(3000, []() {
                QCoreApplication::quit();
            });
        } else {
            // Respawn after 2 seconds if still have lives
            QTimer::singleShot(2000, this, [this]() {
                auto spawn = currentLevel->getSpawn();
                int marioHeight = static_cast<int>(mario->boundingRect().height());
                mario->setPos(spawn.first, spawn.second - marioHeight);
                mario->respawn();
            });
        }
    }
}

void GameController::updateUI() {
    scoreText->setPlainText("Score: " + QString::number(gamePlayer->getScore()));
    livesText->setPlainText("Lives: " + QString::number(gamePlayer->getLives()));
    updateOverlayPositions();
}

void GameController::updateOverlayPositions() {
    if (!mario || !scoreText || !livesText) return;

    QScreen* screen = QApplication::primaryScreen();
    qreal viewportWidth = screen ? screen->geometry().width() : 1280;
    qreal viewportHeight = screen ? screen->geometry().height() : screenHeight;

    qreal cameraCenterX = mario->x() + 40;
    qreal topLeftX = cameraCenterX - viewportWidth / 2.0;
    qreal topLeftY = screenHeight / 2.0 - viewportHeight / 2.0;
    scoreText->setPos(topLeftX + 16, topLeftY + 12);
    livesText->setPos(topLeftX + 16, topLeftY + 40);

    if (endMessageText) {
        qreal centerY = screenHeight / 2.0;
        endMessageText->setPos(
            cameraCenterX - endMessageText->boundingRect().width() / 2.0,
            centerY - endMessageText->boundingRect().height() / 2.0
        );
    }
}

void GameController::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
        luigiMoveLeft = true;

    if (event->key() == Qt::Key_D)
        luigiMoveRight = true;
}

void GameController::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
        luigiMoveLeft = false;

    if (event->key() == Qt::Key_D)
        luigiMoveRight = false;
}
 
bool GameController::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {

        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_A)
            luigiMoveLeft = true;

        if (keyEvent->key() == Qt::Key_D)
            luigiMoveRight = true;
        
            // JUMP
        if (keyEvent->key() == Qt::Key_Space)
            luigiJump = true;
}
    if (event->type() == QEvent::KeyRelease) {

    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_A)
        luigiMoveLeft = false;

    if (keyEvent->key() == Qt::Key_D)
        luigiMoveRight = false;

    if (keyEvent->key() == Qt::Key_Space)
        luigiJump = false;
}

    return QObject::eventFilter(obj, event);
}