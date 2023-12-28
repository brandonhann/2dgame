#include "Player.h"
#include <iostream>

Player::Player(int x, int y) : x(x), y(y), speed(5), movingUp(false), movingDown(false), movingLeft(false), movingRight(false) {
    srcRect = { 0, 0, 32, 32 }; // Size of the texture frame
    destRect = { x, y, 32, 32 }; // Size and position on the screen
}

void Player::update() {
    if (movingUp) y -= speed;
    if (movingDown) y += speed;
    if (movingLeft) x -= speed;
    if (movingRight) x += speed;

    destRect.x = x;
    destRect.y = y;
}

void Player::render(SDL_Renderer* renderer, const SDL_Rect& camera) {
    SDL_Rect renderQuad = {destRect.x - camera.x, destRect.y - camera.y, destRect.w, destRect.h};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the player
    SDL_RenderFillRect(renderer, &renderQuad);
}


void Player::setMovingUp(bool move) {
    movingUp = move;
}

void Player::setMovingDown(bool move) {
    movingDown = move;
}

void Player::setMovingLeft(bool move) {
    movingLeft = move;
}

void Player::setMovingRight(bool move) {
    movingRight = move;
}

int Player::getX() const {
    return x; // Assuming 'x' is the variable holding the player's x-coordinate
}

int Player::getY() const {
    return y; // Assuming 'y' is the variable holding the player's y-coordinate
}
