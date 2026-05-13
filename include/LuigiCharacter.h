#ifndef LUIGI_CHARACTER_H
#define LUIGI_CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>

class LuigiCharacter : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
private:
    bool  isMovingLeft;
    bool  isMovingRight;
    bool  isOnGround;
    float velocityY;

public:
    LuigiCharacter();
    void  startMovingLeft();
    void  startMovingRight();
    void  stopMovingLeft();
    void  stopMovingRight();
    void  jump();
    void  updatePhysics();
    void  setIsOnGround(bool v);
    void  stopUpwardMotion();
    bool  getIsOnGround()    const;
    float getVelocityY()     const;
    bool  getIsMovingLeft()  const;
    bool  getIsMovingRight() const;


};

#endif
