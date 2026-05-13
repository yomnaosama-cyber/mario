#include "LuigiCharacter.h"
#include <QPixmap>

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