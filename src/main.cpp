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
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QFrame>
#include <QFont>
#include "MarioCharacter.h"
#include "GameController.h"
#include "LuigiCharacter.h"

// ──────────────────────────────────────────────────────────
//  Progress helpers  (saves to <appDir>/progress.json)
// ──────────────────────────────────────────────────────────
static QString progressFilePath() {
    return QCoreApplication::applicationDirPath() + "/progress.json";
}

static QJsonObject loadAllProgress() {
    QFile f(progressFilePath());
    if (!f.open(QIODevice::ReadOnly)) return QJsonObject();
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    return doc.isObject() ? doc.object() : QJsonObject();
}

static void saveAllProgress(const QJsonObject& obj) {
    QFile f(progressFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(QJsonDocument(obj).toJson());
}

static QJsonObject getPlayerProgress(const QString& name) {
    QJsonObject all = loadAllProgress();
    return all.contains(name) ? all[name].toObject() : QJsonObject();
}

static void savePlayerProgress(const QString& name, int level, int score, int lives) {
    QJsonObject all = loadAllProgress();
    QJsonObject p;
    p["level"] = level;
    p["score"] = score;
    p["lives"] = lives;
    all[name] = p;
    saveAllProgress(all);
}

// ──────────────────────────────────────────────────────────
//  Sign-in dialog
// ──────────────────────────────────────────────────────────
class SignInDialog : public QDialog {
public:
    QString  selectedName;
    int      startLevel = 1;
    int      startScore = 0;
    int      startLives = 3;

    SignInDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Mario Bros - Player Sign In");
        setFixedSize(480, 540);
        setStyleSheet(
            "QDialog { background: #1a0a00; }"
            "QLabel#title { color: #FFD700; font-size: 26px; font-weight: bold; font-family: Arial; }"
            "QLabel { color: #fff; font-size: 14px; font-family: Arial; }"
            "QLineEdit { background: #3a1a00; color: #fff; border: 2px solid #c8860a; "
            "            border-radius: 6px; padding: 6px 10px; font-size: 14px; }"
            "QPushButton#play { background: #c8860a; color: #fff; border-radius: 8px; "
            "                   font-size: 16px; font-weight: bold; padding: 10px 0; }"
            "QPushButton#play:hover { background: #FFD700; color: #000; }"
            "QPushButton#newgame { background: #2a6a2a; color: #fff; border-radius: 8px; "
            "                   font-size: 14px; padding: 8px 0; }"
            "QPushButton#newgame:hover { background: #4caf50; }"
            "QPushButton#del { background: #6a1a1a; color: #fff; border-radius: 6px; "
            "                  font-size: 12px; padding: 4px 10px; }"
            "QPushButton#del:hover { background: #c0392b; }"
            "QListWidget { background: #2a1200; color: #FFD700; border: 2px solid #c8860a; "
            "              border-radius: 6px; font-size: 13px; }"
            "QListWidget::item:selected { background: #c8860a; color: #fff; }"
        );

        auto* vbox = new QVBoxLayout(this);
        vbox->setContentsMargins(30, 20, 30, 20);
        vbox->setSpacing(12);

        auto* titleLabel = new QLabel("MARIO BROS", this);
        titleLabel->setObjectName("title");
        titleLabel->setAlignment(Qt::AlignCenter);
        vbox->addWidget(titleLabel);

        auto* subLabel = new QLabel("Enter your name to continue or start fresh:", this);
        subLabel->setAlignment(Qt::AlignCenter);
        vbox->addWidget(subLabel);

        nameEdit = new QLineEdit(this);
        nameEdit->setPlaceholderText("Your name...");
        nameEdit->setMaxLength(24);
        vbox->addWidget(nameEdit);

        auto* sep = new QFrame(this);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color: #c8860a;");
        vbox->addWidget(sep);

        auto* savedLabel = new QLabel("Saved players — click to load progress:", this);
        vbox->addWidget(savedLabel);

        savedList = new QListWidget(this);
        savedList->setFixedHeight(150);
        refreshList();
        vbox->addWidget(savedList);

        infoLabel = new QLabel("", this);
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setStyleSheet("color: #aaffaa; font-size: 12px;");
        vbox->addWidget(infoLabel);

        auto* hbox = new QHBoxLayout();
        auto* playBtn = new QPushButton("Play / Continue", this);
        playBtn->setObjectName("play");
        playBtn->setFixedHeight(45);
        hbox->addWidget(playBtn);

        auto* newBtn = new QPushButton("New Game", this);
        newBtn->setObjectName("newgame");
        newBtn->setFixedHeight(45);
        hbox->addWidget(newBtn);
        vbox->addLayout(hbox);

        auto* delBtn = new QPushButton("Delete Save", this);
        delBtn->setObjectName("del");
        delBtn->setFixedHeight(30);
        vbox->addWidget(delBtn);

        auto* hint = new QLabel("Controls:  Mario = Arrow keys + Space    Luigi = A D W", this);
        hint->setAlignment(Qt::AlignCenter);
        hint->setStyleSheet("color: #aaa; font-size: 11px;");
        vbox->addWidget(hint);

        connect(savedList, &QListWidget::currentTextChanged, this, [this](const QString& name) {
            nameEdit->setText(name);
            QJsonObject p = getPlayerProgress(name);
            if (!p.isEmpty()) {
                infoLabel->setText(
                    QString("Level %1  |  Score %2  |  Lives %3")
                    .arg(p["level"].toInt(1))
                    .arg(p["score"].toInt(0))
                    .arg(p["lives"].toInt(3))
                );
            } else {
                infoLabel->setText("");
            }
        });

        connect(playBtn, &QPushButton::clicked, this, [this]() { handlePlay(false); });
        connect(newBtn,  &QPushButton::clicked, this, [this]() { handlePlay(true);  });

        connect(delBtn, &QPushButton::clicked, this, [this]() {
            QString name = nameEdit->text().trimmed();
            if (name.isEmpty()) return;
            QJsonObject all = loadAllProgress();
            if (all.contains(name)) {
                all.remove(name);
                saveAllProgress(all);
                refreshList();
                infoLabel->setText("Save deleted.");
            }
        });
    }

private:
    QLineEdit*   nameEdit;
    QListWidget* savedList;
    QLabel*      infoLabel;

    void refreshList() {
        savedList->clear();
        QJsonObject all = loadAllProgress();
        for (auto it = all.begin(); it != all.end(); ++it) {
            savedList->addItem(it.key());
        }
    }

    void handlePlay(bool forceNew) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "Name required", "Please enter a player name.");
            return;
        }
        selectedName = name;
        QJsonObject p = forceNew ? QJsonObject() : getPlayerProgress(name);
        startLevel = p.contains("level") ? p["level"].toInt() : 1;
        startScore = p.contains("score") ? p["score"].toInt() : 0;
        startLives = p.contains("lives") ? p["lives"].toInt() : 3;
        if (startLives < 1) startLives = 1;
        accept();
    }
};

// ──────────────────────────────────────────────────────────
//  Game view
// ──────────────────────────────────────────────────────────
class GameView : public QGraphicsView
{
private:
    GameController* controller;
    QTimer* gameTimer;
    QString playerName;
    QTimer* autoSaveTimer;

public:
    GameView(QGraphicsScene* scene, GameController* c, const QString& name)
        : QGraphicsView(scene), controller(c), playerName(name) {
        setFocusPolicy(Qt::StrongFocus);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setRenderHint(QPainter::Antialiasing);
        showFullScreen();

        gameTimer = new QTimer(this);
        connect(gameTimer, &QTimer::timeout, this, &GameView::updateGame);
        gameTimer->start(16);

        // Auto-save progress every 10 seconds
        autoSaveTimer = new QTimer(this);
        connect(autoSaveTimer, &QTimer::timeout, this, &GameView::autoSave);
        autoSaveTimer->start(10000);
    }

    void autoSave() {
        if (!controller || !controller->getGamePlayer()) return;
        Player* p = controller->getGamePlayer();
        savePlayerProgress(playerName, controller->getCurrentLevelNumber(),
                           p->getScore(), p->getLives());
        qDebug() << "Progress saved for" << playerName;
    }

    void updateGame() {
        if (controller) {
            controller->updateGame();
            if (controller->getMario()) centerOn(controller->getMario());
        }
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        MarioCharacter* mario = controller ? controller->getMario() : nullptr;
        LuigiCharacter* luigi = controller ? controller->getLuigi() : nullptr;
        if (!mario || mario->getIsDead()) return;

        if (event->key() == Qt::Key_Q) {
            autoSave();
            controller->startNextLevel();
            return;
        }

        switch (event->key()) {
        case Qt::Key_Left:  mario->startMovingLeft();  break;
        case Qt::Key_Right: mario->startMovingRight(); break;
        case Qt::Key_Space:
        case Qt::Key_Up:    mario->jump();             break;
        case Qt::Key_F:
            if (isFullScreen()) showNormal(); else showFullScreen();
            break;
        case Qt::Key_Escape:
            autoSave();
            if (isFullScreen()) showNormal();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (controller) controller->setCanDamageBowser(true);
            break;
        default: break;
        }

        // Luigi: A/D to move, W to jump
        if (luigi) {
            switch (event->key()) {
            case Qt::Key_A: luigi->startMovingLeft();  break;
            case Qt::Key_D: luigi->startMovingRight(); break;
            case Qt::Key_W: luigi->jump();             break;
            default: break;
            }
        }
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        MarioCharacter* mario = controller ? controller->getMario() : nullptr;
        LuigiCharacter* luigi = controller ? controller->getLuigi() : nullptr;

        if (mario) {
            switch (event->key()) {
            case Qt::Key_Left:  mario->stopMovingLeft();  break;
            case Qt::Key_Right: mario->stopMovingRight(); break;
            default: break;
            }
        }
        if (luigi) {
            switch (event->key()) {
            case Qt::Key_A: luigi->stopMovingLeft();  break;
            case Qt::Key_D: luigi->stopMovingRight(); break;
            default: break;
            }
        }
    }
};

// ──────────────────────────────────────────────────────────
//  main
// ──────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MarioBros");

    SignInDialog signIn;
    if (signIn.exec() != QDialog::Accepted) return 0;

    QString playerName = signIn.selectedName;
    int     startLevel = signIn.startLevel;
    int     startScore = signIn.startScore;
    int     startLives = signIn.startLives;

    QScreen* screen = QApplication::primaryScreen();
    int SCREEN_HEIGHT = screen->geometry().height();
    int WORLD_WIDTH   = 4000;
    int TILE_SIZE     = 50;

    QGraphicsScene scene;
    scene.setSceneRect(0, 0, WORLD_WIDTH, SCREEN_HEIGHT);

    QString assetsPath;
    QStringList possiblePaths = {
        QCoreApplication::applicationDirPath() + "/assets",
        QCoreApplication::applicationDirPath() + "/../assets",
        "/home/yomna/mario/assets",
        QDir::currentPath() + "/assets",
        QDir::currentPath() + "/../assets"
    };
    for (const QString& path : possiblePaths) {
        if (QDir(path).exists()) { assetsPath = path; break; }
    }

    QPixmap background;
    if (!assetsPath.isEmpty()) background.load(assetsPath + "/sky1.jpg");
    if (!background.isNull()) {
        scene.setBackgroundBrush(QBrush(background.scaled(
            WORLD_WIDTH, SCREEN_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    } else {
        scene.setBackgroundBrush(QBrush(Qt::cyan));
    }

    GameController controller(&scene, WORLD_WIDTH, SCREEN_HEIGHT, TILE_SIZE,
                              startLevel, startScore, startLives);
    GameView view(&scene, &controller, playerName);
    view.show();

    qDebug() << "Game started:" << playerName << "Level:" << startLevel;
    return app.exec();
}