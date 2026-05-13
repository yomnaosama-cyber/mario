#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QList>
#include <vector>
#include <QTimer>
#include <QString>
#include "MarioCharacter.h"
#include "LuigiCharacter.h"
#include "PiranhaPlant.h"
#include "Player.h"
#include "Enemy.h"
#include "Level.h"
#include "Bowser.h"
#include "HealthBar.h"
#include "Coin.h"
#include "Mushroom.h"
#include "movingplatform.h"

class LockedDoor;

class GameController : public QObject {
    Q_OBJECT
    
private:
    QGraphicsScene* scene;
    MarioCharacter* mario;
    LuigiCharacter* luigi;   // Level 3 character
    Player* gamePlayer;
    Level* currentLevel;
    QList<QGraphicsPixmapItem*> enemyGraphics;
    std::vector<Enemy*> enemies;
    std::vector<QGraphicsPixmapItem*> crumblingGraphics;
    std::vector<Coin*> coins;           // Ensure Coin.h is included
    std::vector<Mushroom*> mushrooms;   // Ensure Mushroom.h is included
    QList<QPair<MovingPlatform*, QGraphicsRectItem*>> movingPlatformGraphics;
    QGraphicsTextItem* keyText;
    QGraphicsRectItem* lockedDoorGraphic;
    QGraphicsPixmapItem* finishItem;
    QGraphicsTextItem* scoreText;
    QGraphicsTextItem* livesText;
    QGraphicsTextItem* levelText;
    QGraphicsTextItem* endMessageText;
    int tileSize;
    int screenHeight;
    int worldWidth;
    bool gameEnded;
    bool levelCompleted;
    int currentLevelNumber;
    int totalLevels;
    int levelnum;
    int keysCollected;

    
    void setupUI();
    void renderTiles();
    void updateUI();
    void updateOverlayPositions();

    LockedDoor* currentLevelDoor = nullptr;
    Bowser* bowser;
    HealthBar* marioHealthBar;
    HealthBar* bowserHealthBar;
    bool bossFightActive;
    bool canDamageBowser;
    QTimer* winZoneTimer;
    QGraphicsPixmapItem* winZoneItem;
    
    void startBossFight();
    void updateBossFight();
    void checkBowserCollision();
    void checkWinZone();
    void showWinZone();
    QString findAssetsPath();
    bool bowserHitCooldown = false;
    bool marioHitCooldown  = false;

public:
    GameController(QGraphicsScene* s, int worldWidth, int screenH, int tileSize,int levelNum, int startScore, int startLives);
    ~GameController();

    void loadLevel(int levelNumber);
    void cleanupCurrentLevel();
    void startNextLevel();
    void updateGame();
    void checkCollisions4();
    bool checkCollisions();
    void handleEnemyCollision(Enemy* enemy);
    void setCanDamageBowser(bool can) { canDamageBowser = can; }
    MarioCharacter* getMario() { return mario; }
    void spawnMushrooms();
    void spawnCoins();
    void checkCoinCollisions();
    void checkMushroomCollisions();
    void skipToNextLevel();
    LuigiCharacter* getLuigi() const { return luigi; }
};

#endif
