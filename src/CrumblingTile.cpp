#include "CrumblingTile.h"

CrumblingTile :: CrumblingTile(long long x, long long y): Tile(x, y, Tile::Brick), state(Normal){
    this->gridX = x;
    this->gridY = y;
}


void CrumblingTile::trigger (){

    this->touch();

}

CrumblingTile::CrumbleState CrumblingTile::getState(){
    return state;
}

CrumblingTile::TileEffect CrumblingTile::touch(){
    if(state!= Normal) return None;

    state = Shaking;

    for(int i = 0; i <10; i++){
        QTimer::singleShot(i*50,this,[this,i](){
            if(state == Shaking){
                int offset = (i%2==0) ? 3: -3;
                this->setPos(this->x() + offset, this->y());
            }


        });
    }

    QTimer::singleShot(500, this, [this](){
        state = Disappear;
        this->setVisible(false);
        this->setY(-1000);
        this->type= Tile::Empty;
    });

    return None;


}
