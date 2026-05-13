#include "LuigiCharacter.h"
#include <QPixmap>
#include <QCoreApplication>
#include <QDir>
#include <QStringList>


static const float GRAVITY = 0.6f;

LuigiCharacter::LuigiCharacter() {
    QPixmap pix("assets/luigi.png");
    setPixmap(pix.scaled(80, 80));
    setZValue(10);

    velocityY = 0;
    isOnGround = false;
}

void LuigiCharacter::updatePhysics() {
    if (!isOnGround) {
        velocityY += GRAVITY;
        setY(y() + velocityY);
    }

    // simple floor clamp (optional safety)
    if (y() > 1000) {
        setY(1000);
        velocityY = 0;
        isOnGround = true;
    }
}
void LuigiCharacter::showWinSprite()
{
    QString assetsPath;

    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        QDir::currentPath() + "/assets"
    };

    for (const QString& path : possiblePaths) {

        QDir dir(path);

        if (dir.exists()) {
            assetsPath = path;
            break;
        }
    }

    QPixmap winPixmap;

    if (!assetsPath.isEmpty()) {

        winPixmap.load(assetsPath + "/luigiwin.png");

        if (!winPixmap.isNull()) {

            QPixmap scaled = winPixmap.scaled(
                80,
                80,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );

            setPixmap(scaled);
        }
    }
}

void LuigiCharacter::setVelocityY(float v) {
    velocityY = v;
}

float LuigiCharacter::getVelocityY() const {
    return velocityY;
}

void LuigiCharacter::setIsOnGround(bool g) {
    isOnGround = g;
}

bool LuigiCharacter::getIsOnGround() const {
    return isOnGround;
}