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
#include "Player.h"
#include "Enemy.h"
#include "Level.h"
#include "Bowser.h"
#include "HealthBar.h"

class GameController : public QObject {
    Q_OBJECT
    
private:
    QGraphicsScene* scene;
    MarioCharacter* mario;
    Player* gamePlayer;
    Level* currentLevel;
    QList<QGraphicsPixmapItem*> enemyGraphics;
    std::vector<Enemy*> enemies;
    std::vector<QGraphicsPixmapItem*> crumblingGraphics;
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
    
    void setupUI();
    void renderTiles();
    void updateUI();
    void updateOverlayPositions();

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
    GameController(QGraphicsScene* s, int worldWidth, int screenH, int tileSize);
    ~GameController();

    void loadLevel(int levelNumber);
    void cleanupCurrentLevel();
    void startNextLevel();
    void updateGame();
    bool checkCollisions();
    void handleEnemyCollision(Enemy* enemy);
    void setCanDamageBowser(bool can) { canDamageBowser = can; }
    MarioCharacter* getMario() { return mario; }
};

#endif
