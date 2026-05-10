#ifndef BOWSER_H
#define BOWSER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QList>

class Bowser : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    
private:
    int direction;
    float speed;
    int minX;
    int maxX;
    float preciseX;
    int yPos;
    int tileSize;
    int health = 5;
    int maxHealth = 5;
    bool isDead;
    bool isVulnerable;
    QVector<QPixmap> runFramesLeft;
    QVector<QPixmap> fireFramesLeft;
    QPixmap dyingFrameLeft;
    
    QList<QPixmap> runFrames;
    QList<QPixmap> fireFrames;
    QPixmap dyingFrame;
    QTimer* animationTimer;
    int currentFrame;
    QString currentState;
    
    QTimer* shootTimer;
    bool isShooting;
    
public:
    Bowser(int x, int y, int minX, int maxX, int tileSize, float moveSpeed = 0.15f);
    ~Bowser();
    
    void updateMovement();
    void shootFire();
    void takeDamage(int amount);
    void die();
    void nextFrame();
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setVulnerable(bool vulnerable) { isVulnerable = vulnerable; }
    bool getIsDead() const { return isDead; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getPreciseX() const { return preciseX; }
    int getY() const { return yPos; }
    bool isFacingLeft() const { return direction == -1; }
    bool getIsShooting() const { return isShooting; }
    bool getIsVulnerable() const;    
signals:
    void healthChanged(int current, int max);
    void died();
};

#endif