#ifndef MARIO_CHARACTER_H
#define MARIO_CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QList>
#include <QDebug>

class MarioCharacter : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    
private:
    QPixmap standingFrame;
    QList<QPixmap> runningFrames;
    QPixmap jumpingFrame;
    QPixmap dyingFrame;
    
    QTimer *animationTimer;
    int currentFrame;
    QString currentState;
    
    bool isMovingLeft;
    bool isMovingRight;
    bool isJumping;
    bool isDead;
    
    qreal velocityY;
    bool isOnGround;
    
public:
    MarioCharacter();
    void nextFrame();
    void startMovingLeft();
    void startMovingRight();
    void stopMovingLeft();
    void stopMovingRight();
    void jump();
    void updatePhysics();
    void die();
    void respawn();
    bool getIsDead() const;
    void setIsOnGround(bool ground) { isOnGround = ground; }
    void stopUpwardMotion();
};

#endif