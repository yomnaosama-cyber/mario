#ifndef COIN_H
#define COIN_H

#include <QGraphicsEllipseItem>

struct Coin {
    int tileX;
    int tileY;
    bool collected;
    QGraphicsEllipseItem* graphic;

    Coin(int x, int y) : tileX(x), tileY(y), collected(false), graphic(nullptr) {}
};

#endif
