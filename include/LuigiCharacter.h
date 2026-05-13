#ifndef LUIGI_CHARACTER_H
#define LUIGI_CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QCoreApplication>
#include <QDir>
#include <QStringList>

class LuigiCharacter : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    LuigiCharacter();

    // sPHYSICS
    void updatePhysics();
    void showWinSprite();

    void setVelocityY(float v);
    float getVelocityY() const;

    void setIsOnGround(bool g);
    bool getIsOnGround() const;

private:
    float velocityY;
    bool isOnGround;
};

#endif