#include "Player.h"

Player::Player(int x, int y)
    : Entity(x, y, 100, "Player"), lives(3), score(0) {}

// FIX 9: Removed redundant move() override — Entity::move() is sufficient.
// Add boundary/physics logic here later if needed.

void Player::addScore(int points) {
    score += points;
}

void Player::loseLife() {
    lives--;
    if (lives < 0) lives = 0;
}

int Player::getLives() const { return lives; }
int Player::getScore() const { return score; }