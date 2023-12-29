#include "Player.h"
#include <SDL_image.h>
#include <iostream>

const float Player::BIOME_CHANGE_COOLDOWN = 1.0f;
SDL_Texture* Player::playerTexture = nullptr;

Player::Player(int x, int y) : x(x), y(y), speed(5), currentBiome(""), lastBiome(""), timeSinceLastBiomeChange(0.0f) {    
    srcRect = { 0, 0, 32, 64 }; // Update the size of the texture frame to 32x64
    destRect = { x, y, 32, 64 }; // Update the size and position on the screen to 32x64
}

void Player::update(float deltaTime) {
    if (movingUp) y -= speed;
    if (movingDown) y += speed;
    if (movingLeft) x -= speed;
    if (movingRight) x += speed;

    destRect.x = x;
    destRect.y = y;
}

void Player::handleInput(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                setMovingUp(true);
                break;
            case SDLK_s:
                setMovingDown(true);
                break;
            case SDLK_a:
                setMovingLeft(true);
                break;
            case SDLK_d:
                setMovingRight(true);
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
            case SDLK_w:
                setMovingUp(false);
                break;
            case SDLK_s:
                setMovingDown(false);
                break;
            case SDLK_a:
                setMovingLeft(false);
                break;
            case SDLK_d:
                setMovingRight(false);
                break;
        }
    }
}

void Player::render(SDL_Renderer* renderer, const SDL_Rect& camera) {
    if (!playerTexture) {
        std::cerr << "Player texture not loaded." << std::endl;
        return;
    }

    SDL_Rect renderQuad = {destRect.x - camera.x, destRect.y - camera.y, destRect.w, destRect.h};
    
    // Render the player texture instead of a red square
    SDL_RenderCopy(renderer, playerTexture, &srcRect, &renderQuad);
}

void Player::loadPlayerTexture(SDL_Renderer* renderer, const char* filePath) {
    // Load the texture from a file
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, filePath);
    if(newTexture == nullptr) {
        std::cerr << "Failed to load player texture: " << IMG_GetError() << std::endl;
        return;
    }
    // Assign the new texture to the playerTexture member
    playerTexture = newTexture;
}

void Player::destroyTexture() {
    if (playerTexture != nullptr) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
    }
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
