/*#include "LuigiCharacter.h"
#include <QPixmap>

LuigiCharacter::LuigiCharacter() {
    QPixmap pix("assets/luigi.png");
    setPixmap(pix.scaled(80, 80));
    setZValue(10);
*/


#include "LuigiCharacter.h"
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QTransform>
#include <QDebug>
    LuigiCharacter :: LuigiCharacter() : isMovingLeft(false), isMovingRight(false),
        isOnGround(true), velocityY(0.0f)
    {
        QString assetsPath;
        QStringList possiblePaths = {
            QCoreApplication::applicationDirPath() + "/assets",
            QCoreApplication::applicationDirPath() + "/../assets",
            "/home/yomna/mario/assets",
            QDir::currentPath() + "/assets",
            QDir::currentPath() + "/../assets"
        };
        for (const QString& path : possiblePaths) {
            if (QDir(path).exists()) {
                assetsPath = path;
                break;
            }
        }
        QPixmap pix;
        if (!assetsPath.isEmpty()) {
            pix.load(assetsPath + "/luigi-Photoroom.png");
        }
        if (pix.isNull()) {
            pix = QPixmap(80, 80);
            pix.fill(QColor(34, 139, 34));
        }
        setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        setZValue(10);
    }
    void LuigiCharacter::startMovingLeft() {
        isMovingLeft  = true;
        isMovingRight = false;
        setTransform(QTransform(-1, 0, 0, 1, 80, 0));
    }
    void LuigiCharacter::startMovingRight() {
        isMovingRight = true;
        isMovingLeft  = false;
        setTransform(QTransform(1, 0, 0, 1, 0, 0));
    }
    void LuigiCharacter::stopMovingLeft()  { isMovingLeft  = false; }
    void LuigiCharacter::stopMovingRight() { isMovingRight = false; }
    void LuigiCharacter::jump() {
        if (isOnGround) {
            isOnGround = false;
            velocityY  = -15.0f;
        }
    }
    void LuigiCharacter::updatePhysics() {
        if (!isOnGround) {
            velocityY += 0.8f;
            if (velocityY > 15.0f) velocityY = 15.0f;
            setY(y() + velocityY);
        } else {
            if (velocityY > 0) velocityY = 0;
        }
        const int MOVE_SPEED = 8;
        if (isMovingLeft)  setX(x() - MOVE_SPEED);
        if (isMovingRight) setX(x() + MOVE_SPEED);
    }
    void LuigiCharacter::setIsOnGround(bool v) {
        isOnGround = v;
        if (v && velocityY > 0) velocityY = 0;
    }
    void LuigiCharacter::stopUpwardMotion() {
        if (velocityY < 0) velocityY = 0;
    }
    bool  LuigiCharacter::getIsOnGround()   const { return isOnGround;   }
    float LuigiCharacter::getVelocityY()    const { return velocityY;    }
    bool  LuigiCharacter::getIsMovingLeft()  const { return isMovingLeft;  }
    bool  LuigiCharacter::getIsMovingRight() const { return isMovingRight; }

