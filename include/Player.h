#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <string>

class Player : public Entity {
private:
    int lives;
    int score;

public:
    Player(int x, int y);

    // FIX 9: move() is inherited from Entity; override removed (add back with boundary logic if needed)

    void addScore(int points);
    void loseLife();

    int getLives() const;
    int getScore() const;
};

#endif