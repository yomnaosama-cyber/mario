#ifndef MUSHROOM_H
#define MUSHROOM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsItem>

struct Mushroom {
    int tileX;
    int tileY;
    bool collected;
    QGraphicsItem* graphic;

    Mushroom(int x, int y) : tileX(x), tileY(y), collected(false), graphic(nullptr) {}
};

#endif
