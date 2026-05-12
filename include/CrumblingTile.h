#ifndef CRUMBLING_TILE_H
#define CRUMBLING_TILE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QList>
#include <QDebug>
#include "Tile.h"

class CrumblingTile : public Tile, public QObject{

    Q_OBJECT


    public:
        enum CrumbleState {Normal, Shaking, Disappear};

    private:
        CrumbleState state;


    public:
        CrumblingTile(long long x, long long y);

        void trigger ();
        CrumbleState getState();
        TileEffect touch() override;

};


#endif
