#include "Bowser.h"
#include <QTransform>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

Bowser::Bowser(int x, int y, int minX, int maxX, int tileSize, float moveSpeed)
    : direction(-1), speed(moveSpeed), minX(minX), maxX(maxX), 
      preciseX(static_cast<float>(x)), yPos(y), tileSize(tileSize),
      health(5), maxHealth(5), isDead(false), isVulnerable(false),
      currentFrame(0), currentState("running"), isShooting(false) {
    
    QString assetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        "/home/yomna/mario/assets",
        QDir::currentPath() + "/assets",
        QDir::currentPath() + "/../assets"
    };
    
    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            assetsPath = path;
            break;
        }
    }
    
    for (int i = 1; i <= 5; i++) {
        QPixmap run(assetsPath + "/brun" + QString::number(i) + ".png");
        if (!run.isNull()) {
            runFrames.append(run.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            qDebug() << "Loaded brun" << i << ".png";
        }
    }
    
    for (int i = 1; i <= 3; i++) {
        QPixmap fire(assetsPath + "/bf" + QString::number(i) + ".png");
        if (!fire.isNull()) {
            fireFrames.append(fire.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            qDebug() << "Loaded bf" << i << ".png";
        }
    }
    
    QPixmap dying(assetsPath + "/bdying.png");
    if (!dying.isNull()) {
        dyingFrame = dying.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Loaded bdying.png";
    }
    
    if (runFrames.isEmpty()) {
        QPixmap fallback(100, 100);
        fallback.fill(Qt::darkRed);
        for (int i = 0; i < 5; i++) runFrames.append(fallback);
    }
    if (fireFrames.isEmpty()) {
        QPixmap fallback(100, 100);
        fallback.fill(Qt::red);
        for (int i = 0; i < 3; i++) fireFrames.append(fallback);
    }
    if (dyingFrame.isNull()) {
        QPixmap fallback(100, 100);
        fallback.fill(Qt::black);
        dyingFrame = fallback;
    }

    // Pre-mirror every frame so we never need a runtime QTransform for flipping.
    for (const QPixmap& p : runFrames)
        runFramesLeft.append(p.transformed(QTransform().scale(-1, 1)));
    for (const QPixmap& p : fireFrames)
        fireFramesLeft.append(p.transformed(QTransform().scale(-1, 1)));
    dyingFrameLeft = dyingFrame.transformed(QTransform().scale(-1, 1));

    setPixmap(runFramesLeft[0]);   // starts facing left (direction = -1)
    setZValue(15);
    // Ground tile top = yPos * tileSize; sprite is 100 px tall → top at groundTop - 100
    setPos(preciseX * tileSize, yPos * tileSize - 100);
    
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Bowser::nextFrame);
    animationTimer->start(100);
    
    shootTimer = new QTimer(this);
    connect(shootTimer, &QTimer::timeout, this, &Bowser::shootFire);
    shootTimer->start(2500);
}

Bowser::~Bowser() {
    delete animationTimer;
    delete shootTimer;
}

void Bowser::updateMovement() {
    if (isDead) return;

    int prevDirection = direction;
    preciseX += direction * speed;

    if (preciseX <= minX) {
        preciseX = static_cast<float>(minX);
        direction = 1;
    } else if (preciseX >= maxX) {
        preciseX = static_cast<float>(maxX);
        direction = -1;
    }

    if (direction != prevDirection) {
        isVulnerable = true;
        QTimer::singleShot(600, this, [this]() { isVulnerable = false; });
        qDebug() << "Bowser turned - VULNERABLE!";
        // Immediately show the correct mirrored frame so flip is instant, not gradual
        if (!isShooting) {
            const QVector<QPixmap>& frames = (direction == 1) ? runFrames : runFramesLeft;
            if (!frames.isEmpty())
                setPixmap(frames[currentFrame % frames.size()]);
        }
    }

    // Ground tile top pixel = yPos * tileSize; sprite height = 100px → top at groundTop - 100
    setPos(preciseX * tileSize, yPos * tileSize - 100);
}

bool Bowser::getIsVulnerable() const {
    return isVulnerable;
}

void Bowser::shootFire() {
    if (isDead) return;
    if (!isShooting) {
        isShooting = true;
        currentState = "shooting";
        currentFrame = 0;
        qDebug() << "Bowser shoots fire!";
    }
}

void Bowser::takeDamage(int amount) {
    if (isDead) return;
    health -= amount;
    if (health < 0) health = 0;
    emit healthChanged(health, maxHealth);
    qDebug() << "Bowser hit! Health:" << health << "/" << maxHealth;
    
    if (health <= 0) {
        die();
    }
}

void Bowser::die() {
    if (!isDead) {
        isDead = true;
        isShooting = false;
        animationTimer->stop();
        shootTimer->stop();
        setPixmap((direction == 1) ? dyingFrame : dyingFrameLeft);
        emit died();
        qDebug() << "Bowser defeated!";
    }
}

void Bowser::nextFrame() {
    if (isDead) return;

    if (isShooting) {
        // Pick fire list based on facing direction — no transform needed
        const QVector<QPixmap>& frames = (direction == 1) ? fireFrames : fireFramesLeft;
        if (!frames.isEmpty()) {
            currentFrame = (currentFrame + 1) % frames.size();
            setPixmap(frames[currentFrame]);
            if (currentFrame == frames.size() - 1) {
                isShooting = false;
                currentState = "running";
                currentFrame = 0;
            }
        }
    } else {
        // Pick run list based on facing direction — no transform needed
        const QVector<QPixmap>& frames = (direction == 1) ? runFrames : runFramesLeft;
        if (!frames.isEmpty()) {
            currentFrame = (currentFrame + 1) % frames.size();
            setPixmap(frames[currentFrame]);
        }
    }
}