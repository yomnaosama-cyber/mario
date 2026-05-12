#ifndef LUIGI_CHARACTER_H
#define LUIGI_CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>

class LuigiCharacter : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    LuigiCharacter();
};

#endif
