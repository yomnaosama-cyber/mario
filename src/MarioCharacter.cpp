#include "MarioCharacter.h"
#include <QTransform>
#include <QDir>
#include <QCoreApplication>

MarioCharacter::MarioCharacter() : QObject(), QGraphicsPixmapItem(), currentFrame(0), isMovingLeft(false), isMovingRight(false), 
                   isJumping(false), isDead(false), velocityY(0), isOnGround(true) {
    
    // Determine the assets path (works from both build and source directories)
    QString assetsPath;
    
    // Try to find assets in different possible locations
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",  // build/assets (after copy)
        QCoreApplication::applicationDirPath() + "/../assets", // if executable is in build/bin
        "/home/yomna/mario/assets",  // absolute source path
        QDir::currentPath() + "/assets",  // current working directory
        QDir::currentPath() + "/../assets"  // one level up
    };
    
    for (const QString& path : possiblePaths) {
        QDir dir(path);
        if (dir.exists()) {
            assetsPath = path;
            qDebug() << "Found assets at:" << assetsPath;
            break;
        }
    }
    
    if (assetsPath.isEmpty()) {
        qDebug() << "WARNING: Could not find assets folder! Using fallback colors.";
        assetsPath = QCoreApplication::applicationDirPath();
    }
    
    // Load STANDING frame
    QPixmap standing(assetsPath + "/standing.png");
    if (!standing.isNull()) {
        standingFrame = standing.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Loaded standing.png";
    }
    
    // Load RUNNING frames
    for (int i = 1; i <= 3; i++) {
        QPixmap run(assetsPath + "/run" + QString::number(i) + ".png");
        if (!run.isNull()) {
            runningFrames.append(run.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            qDebug() << "Loaded run" << i << ".png";
        }
    }
    
    // Load JUMPING frame
    QPixmap jumping(assetsPath + "/jumping.png");
    if (!jumping.isNull()) {
        jumpingFrame = jumping.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Loaded jumping.png";
    }
    
    // Load DYING frame
    QPixmap dying(assetsPath + "/dying.png");
    if (!dying.isNull()) {
        dyingFrame = dying.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qDebug() << "Loaded dying.png";
    }
    
    // Fallback if no images found - create colored rectangles
    if (standingFrame.isNull()) {
        qDebug() << "No sprite images found! Creating fallback colored rectangles.";
        QPixmap fallback(80, 80);
        fallback.fill(Qt::red);
        standingFrame = fallback;
        
        QPixmap runFallback(80, 80);
        runFallback.fill(Qt::green);
        for (int i = 0; i < 3; i++) {
            runningFrames.append(runFallback);
        }
        
        QPixmap jumpFallback(80, 80);
        jumpFallback.fill(Qt::yellow);
        jumpingFrame = jumpFallback;
        
        QPixmap dieFallback(80, 80);
        dieFallback.fill(Qt::black);
        dyingFrame = dieFallback;
    }
    
    // Ensure we have at least one running frame
    if (runningFrames.isEmpty()) {
        runningFrames.append(standingFrame);
        runningFrames.append(standingFrame);
        runningFrames.append(standingFrame);
    }
    
    // Fill missing running frames if we have some but not all
    while (runningFrames.size() < 3) {
        runningFrames.append(runningFrames.last());
    }
    
    // Set initial sprite (position will be set by GameController)
    setPixmap(standingFrame);
    setZValue(10);
    
    // Setup animation timer
    animationTimer = new QTimer(this);
    QObject::connect(animationTimer, &QTimer::timeout, this, &MarioCharacter::nextFrame);
    animationTimer->start(80);  // Start the timer so animation runs

    currentState = "standing";
    qDebug() << "MarioCharacter initialized successfully!";
}

void MarioCharacter::nextFrame() {
    if (isDead) {
        if (!dyingFrame.isNull()) {
            setPixmap(dyingFrame);
        }
        return;
    }
    
    if (!isOnGround) {
        // Jumping state
        if (currentState != "jumping") {
            currentState = "jumping";
        }
        if (!jumpingFrame.isNull()) {
            setPixmap(jumpingFrame);
        }
    }
    else if (isMovingLeft || isMovingRight) {
        // Running state
        if (currentState != "running") {
            currentState = "running";
            currentFrame = 0;
        }
        
        if (!runningFrames.isEmpty()) {
            currentFrame = (currentFrame + 1) % runningFrames.size();
            setPixmap(runningFrames[currentFrame]);
        }
    }
    else {
        // Standing state
        if (currentState != "standing") {
            currentState = "standing";
            if (!standingFrame.isNull()) {
                setPixmap(standingFrame);
            }
        }
    }
}

void MarioCharacter::startMovingLeft() {
    if (isDead) return;
    isMovingLeft = true;
    isMovingRight = false;
    // Mirror around the sprite center (width=80)
    setTransform(QTransform(-1, 0, 0, 1, 80, 0));
}

void MarioCharacter::startMovingRight() {
    if (isDead) return;
    isMovingRight = true;
    isMovingLeft = false;
    // Reset transform to normal orientation
    setTransform(QTransform(1, 0, 0, 1, 0, 0));
}

void MarioCharacter::stopMovingLeft() {
    isMovingLeft = false;
    // Return to standing frame if not moving right
    if (!isMovingRight && !isDead && isOnGround) {
        currentState = "standing";
        setPixmap(standingFrame);
    }
}

void MarioCharacter::stopMovingRight() {
    isMovingRight = false;
    // Return to standing frame if not moving left
    if (!isMovingLeft && !isDead && isOnGround) {
        currentState = "standing";
        setPixmap(standingFrame);
    }
}

void MarioCharacter::jump() {
    if (!isDead && isOnGround) {
        isJumping = true;
        isOnGround = false;
        velocityY = -15;  // Negative = upward
        currentState = "jumping";
        if (!jumpingFrame.isNull()) {
            setPixmap(jumpingFrame);
        }
        qDebug() << "Mario jumped!";
    }
}

void MarioCharacter::updatePhysics() {
    if (isDead) return;

    // Apply gravity when airborne
    if (!isOnGround) {
        velocityY += 0.8;  // Gravity
        if (velocityY > 15) velocityY = 15;  // Terminal velocity
        setY(y() + velocityY);
    } else {
        // Reset vertical velocity when on ground
        if (velocityY > 0) velocityY = 0;
        isJumping = false;
    }

    // Horizontal movement
    const int MOVE_SPEED = 8;
    if (isMovingLeft) {
        setX(x() - MOVE_SPEED);
    }
    if (isMovingRight) {
        setX(x() + MOVE_SPEED);
    }
    
}

void MarioCharacter::die() {
    if (!isDead) {
        isDead = true;
        isMovingLeft = false;
        isMovingRight = false;
        isJumping = false;
        velocityY = 0;
        
        if (!dyingFrame.isNull()) {
            setPixmap(dyingFrame);
        }
        qDebug() << "Player died!";
    }
}

void MarioCharacter::respawn() {
    isDead = false;
    isMovingLeft = false;
    isMovingRight = false;
    isJumping = false;
    isOnGround = true;
    velocityY = 0;
    
    // Reset transform to face right
    setTransform(QTransform(1, 0, 0, 1, 0, 0));
    
    // Set standing sprite
    if (!standingFrame.isNull()) {
        setPixmap(standingFrame);
    }
    
    currentState = "standing";
    animationTimer->start(80);
    
    qDebug() << "Player respawned!";
}
bool MarioCharacter::getIsDead() const {
    return isDead;
}

void MarioCharacter::stopUpwardMotion() {
    if (velocityY < 0) {
        velocityY = 0;
    }
}

float MarioCharacter::getVelocityY() const {
    return velocityY;
}

void MarioCharacter::bounceUp() {
    // Give Mario a small upward bounce after stomping Bowser
    velocityY = -10;
    isOnGround = false;
    isJumping = true;
}