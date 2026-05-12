#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QScreen>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include "MarioCharacter.h"
#include "GameController.h"

class GameView : public QGraphicsView
{
private:
    GameController* controller;
    QTimer* gameTimer;
    
public:
    GameView(QGraphicsScene* scene, GameController* c) : QGraphicsView(scene), controller(c) {
        setFocusPolicy(Qt::StrongFocus);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setRenderHint(QPainter::Antialiasing);
        
        showFullScreen();
        
        gameTimer = new QTimer(this);
        connect(gameTimer, &QTimer::timeout, this, &GameView::updateGame);
        gameTimer->start(16);
    }
    
    void updateGame() {
        if (controller) {
            controller->updateGame();
            if (controller->getMario()) {
                centerOn(controller->getMario());
            }
        }
    }
    
protected:
    void keyPressEvent(QKeyEvent* event) override {
        MarioCharacter* mario = controller ? controller->getMario() : nullptr;
        if (!mario || mario->getIsDead()) return;
        
        switch (event->key()) {
            case Qt::Key_Left:
                mario->startMovingLeft();
                break;
            case Qt::Key_Right:
                mario->startMovingRight();
                break;
            case Qt::Key_Space:
            case Qt::Key_Up:
                mario->jump();
                break;
            case Qt::Key_F:
                if (isFullScreen()) showNormal();
                else showFullScreen();
                break;
            case Qt::Key_Escape:
                if (isFullScreen()) showNormal();
                break;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (controller) {
                    controller->setCanDamageBowser(true);
                }
                break;
        }
    }
    
    void keyReleaseEvent(QKeyEvent* event) override {
        MarioCharacter* mario = controller ? controller->getMario() : nullptr;
        if (!mario) return;
        
        switch (event->key()) {
            case Qt::Key_Left:
                mario->stopMovingLeft();
                break;
            case Qt::Key_Right:
                mario->stopMovingRight();
                break;
        }
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    QScreen* screen = QApplication::primaryScreen();
    int SCREEN_HEIGHT = screen->geometry().height();
    int WORLD_WIDTH = 4000;
    int TILE_SIZE = 50;
    
    QGraphicsScene scene;
    scene.setSceneRect(0, 0, WORLD_WIDTH, SCREEN_HEIGHT);
    
    // Find assets folder and load background
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
            qDebug() << "Found assets for background at:" << assetsPath;
            break;
        }
    }
    
    // Load background image
    QPixmap background;
    if (!assetsPath.isEmpty()) {
        background.load(assetsPath + "/sky1.jpg");
    }
    
    if (!background.isNull()) {
        background = background.scaled(WORLD_WIDTH, SCREEN_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        scene.setBackgroundBrush(QBrush(background));
        qDebug() << "Background loaded successfully!";
    } else {
        scene.setBackgroundBrush(QBrush(Qt::cyan));
        qDebug() << "Background image not found - using cyan background";
        qDebug() << "Expected sky1.jpg in assets folder";
    }
    
    GameController controller(&scene, WORLD_WIDTH, SCREEN_HEIGHT, TILE_SIZE);
    GameView view(&scene, &controller);
    view.show();
    
    qDebug() << "Game Started! Use arrow keys to move, Space/Up to jump";
    qDebug() << "Press F for fullscreen, Escape to exit fullscreen";
    
    return app.exec();
}
