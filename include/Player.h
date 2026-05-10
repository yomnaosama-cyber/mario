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

    void addScore(int points);
    void loseLife();
    void setLives(int newLives);

    void setPosition(int x, int y) { this->x = x; this->y = y; }

    void takeDamage(int amount) { 
        health -= amount; 
        if (health < 0) health = 0; 
    }

    int getHealth() const { return health; }

    void setHealth(int h) { health = h; }

    int getLives() const;
    int getScore() const;
};

#endif