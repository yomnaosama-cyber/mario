#ifndef TILE_H
#define TILE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>


class Tile : public QObject, public QGraphicsPixmapItem{
    Q_OBJECT
  public:
    int gridX;
    int gridY;
  enum Tiletype {
    Brick,
    Grass,
    Mud,
    Steel,
    MysteryBox,
    Flag,
    Empty,
    Door
  };

  enum TileEffect {
    None,
    Deadly,
    Prize,
    Next_Level,
    Key_Collected
  };
   
  Tiletype type;
  
  void setupAppearance() {
      setPixmap(QPixmap(":/assets/brick.png"));
      setPos(gridX * 32, gridY * 32); // Maps Grid to Pixels
  }
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

class LockedDoor : public Tile{
  public:
    int keys;
    LockedDoor(long long x, long long y, Tiletype type, int Coinsneeded);
    TileEffect touch() override;
    void opendoor(int x);
    void addKey();
    int getkeyscollected();
    int getkeysneeded();
    bool isOpen();
    int getrow();
    int getcol();
    void opendoor();

  private:

  int CoinsNeeded;
  bool isDOpen;


};

class SecretCoinTile : public Tile{
private:
    bool collected;
public:
    SecretCoinTile(long long x, long long y);
    TileEffect touch() override;


};

#endif
