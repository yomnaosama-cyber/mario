#include "Tile.h"
  
  Tile :: Tile (long long x, long long y, Tile::Tiletype type) {
      this->gridX=x;
      this->gridY=y;
      this->type = type;

      setPos(x * 32, y * 32);
  }

  Tile :: Tile () : gridX(0), gridY(0), type(Empty){}

  Tile::TileEffect Tile::touch(){
    return Tile::None;
  }

  

  DeadlyTile :: DeadlyTile(long long x, long long y, Tile::Tiletype type) : Tile(x, y, type) {}

  Tile::TileEffect DeadlyTile::touch() {
    return Tile::Deadly;
  }


  InteractiveTile::InteractiveTile(long long x, long long y, Tile::Tiletype type, InteractiveTile::PrizeType prize) : Tile(x, y, type), prize (prize) {}

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
 

   EndTile :: EndTile(long long x, long long y, Tile::Tiletype type) : Tile(x, y, type) {}

  Tile::TileEffect EndTile:: touch() {
    return Tile::Next_Level;
  }




  LockedDoor :: LockedDoor(long long x, long long y, Tile::Tiletype type, int Coinsneeded) : Tile(x,y,type), CoinsNeeded(Coinsneeded),isDOpen(false) {}

  Tile::TileEffect LockedDoor::touch() {
      if(isDOpen == false){
        return Tile::Deadly;
      }
      else{
        return Tile::None;
      }


  }

  void LockedDoor :: opendoor(){
    if(CoinsNeeded == keys){
      isDOpen = true;
    }
  }

  void LockedDoor :: addKey(){
      keys +=1;
  }

  int LockedDoor:: getkeyscollected(){
      return keys;
  }

  int LockedDoor:: getkeysneeded(){
      return CoinsNeeded - keys;
  }

  bool LockedDoor:: isOpen(){
      return isDOpen;
  }

  int LockedDoor::getrow(){
      return static_cast<int>(gridY);
  }

  int LockedDoor::getcol(){
      return static_cast<int>(gridX);
  }
  SecretCoinTile :: SecretCoinTile(long long x, long long y) : Tile(x,y,Tile::MysteryBox), collected(false){}

  SecretCoinTile::TileEffect SecretCoinTile::touch(){
      if(collected == false){
          collected = true;
          return Tile::Key_Collected;
      }
      else{

          return Tile::None;

      }

  }



  

