#include "Tile.h"
  
  Tile :: Tile (long long x, long long y, Tile::Tiletype type) : x(x), y(y), type(type) {};

  Tile :: Tile () : x(0), y(0), type(Empty){}

  Tile::TileEffect Tile::touch(){
    return Tile::None;
  }

  

  DeadlyTile :: DeadlyTile(long long x, long long y, Tile::Tiletype type) : Tile(x, y, type) {};

  Tile::TileEffect DeadlyTile::touch() {
    return Tile::Deadly;
  }


  InteractiveTile::InteractiveTile(long long x, long long y, Tile::Tiletype type, InteractiveTile::PrizeType prize) : Tile(x, y, type), prize (prize) {};

  Tile::TileEffect InteractiveTile::touch() {
    if(hit == 0){
      hit+=1;
      return Tile::Prize;
    }
    else{
      
      return Tile::None;
      
    }
  }

  InteractiveTile::PrizeType InteractiveTile::getPrizeType(){
    return prize;
  }
 

   EndTile :: EndTile(long long x, long long y, Tile::Tiletype type) : Tile(x, y, type) {};

  Tile::TileEffect EndTile:: touch() {
    return Tile::Next_Level;
  }

