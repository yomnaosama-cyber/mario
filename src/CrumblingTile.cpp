#include "CrumblingTile.h"

CrumblingTile :: CrumblingTile(long long x, long long y): Tile(x, y, Tile::Brick), state(Normal){}


void CrumblingTile::trigger (){
    state = Shaking;

    QTimer::singleShot(500, this, [this](){
        state = Disappear;
    });

}

CrumblingTile::CrumbleState CrumblingTile::getState(){
    return state;
}

CrumblingTile::TileEffect CrumblingTile::touch(){

    if(state == Normal){
        return None;
    }
    else if(state == Shaking){
        return Deadly;
    }
    else{
        return Deadly;
    }

}
