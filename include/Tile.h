#ifndef TILE_H
#define TILE_H

class Tile{
  public:
  long long x, y;
  enum Tiletype {
    Brick,
    Grass,
    Mud,
    Steel,
    MysteryBox,
    Flag,
    Empty,
  };

  enum TileEffect {
    None,
    Deadly,
    Prize,
    Next_Level,
  };
   
  Tiletype type;
  
  Tile(long long x, long long y, Tiletype type);
  Tile();
  virtual ~Tile() = default;
  virtual TileEffect touch();
  
  Tiletype getType() const { return type; }  // ADD THIS
};

class DeadlyTile : public Tile {
  public:
  DeadlyTile(long long x, long long y, Tiletype type);
  TileEffect touch () override;
};

class InteractiveTile : public Tile{
  public:
  enum PrizeType{  // MOVED TO PUBLIC
    Coins,
    PowerUp,
  };

  private:
  int hit = 0;
  PrizeType prize;
  
  public:
  InteractiveTile(long long x, long long y, Tiletype type, PrizeType Prize);
  TileEffect touch() override;
  PrizeType getPrizeType();
};

class EndTile : public Tile{
  public:
  EndTile(long long x, long long y, Tiletype type);
  TileEffect touch() override;
};

#endif