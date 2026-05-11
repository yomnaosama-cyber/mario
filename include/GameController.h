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
    QGraphicsPixmapItem* finishItem;
    QGraphicsTextItem* scoreText;
    QGraphicsTextItem* livesText;
    QGraphicsTextItem* endMessageText;
    int tileSize;
    int screenHeight;
    int worldWidth;
    bool gameEnded;
    bool isLevel3;
    
    void setupUI();
    void renderTiles();
    void updateUI();
    void updateOverlayPositions();
    
public:
    GameController(QGraphicsScene* s, int worldWidth, int screenH, int tileSize);
    ~GameController();
    
    void updateGame();
    bool checkCollisions();
    void handleEnemyCollision(Enemy* enemy);
    MarioCharacter* getMario() { return mario; }
};

#endif