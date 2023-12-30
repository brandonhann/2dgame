#include "Player.h"
#include <SDL_image.h>
#include <iostream>

const float Player::BIOME_CHANGE_COOLDOWN = 1.0f;
SDL_Texture* Player::playerTexture = nullptr;

Player::Player(int x, int y) : x(x), y(y), speed(5), frameIndex(0), frameTime(0.0f), animationSpeed(0.1f) {
    idleSrcRect = { 0, 0, 32, 32 };
    walkingSrcRects[0][0] = { 32, 0, 32, 32 }; // Down
    walkingSrcRects[0][1] = { 64, 0, 32, 32 }; // Down
    walkingSrcRects[3][0] = { 96, 0, 32, 32 }; // Right
    walkingSrcRects[3][1] = { 128, 0, 32, 32 }; // Right
    walkingSrcRects[2][0] = { 160, 0, 32, 32 }; // Up
    walkingSrcRects[2][1] = { 192, 0, 32, 32 }; // Up
    walkingSrcRects[1][0] = { 224, 0, 32, 32 }; // Left
    walkingSrcRects[1][1] = { 256, 0, 32, 32 }; // Left
    destRect = { x, y, 32, 32 };
}

void Player::updateAnimation(float deltaTime) {
    frameTime += deltaTime;
    if (frameTime >= animationSpeed) {
        frameIndex = (frameIndex + 1) % 2; // Assuming 2 frames per direction
        frameTime = 0.0f;
    }

    int direction = 0; // 0: down, 1: left, 2: up, 3: right
    if (movingUp) {
        direction = 2;
    } else if (movingDown) {
        direction = 0;
    } else if (movingLeft) {
        direction = 1;
    } else if (movingRight) {
        direction = 3;
    }

    if (movingUp || movingDown || movingLeft || movingRight) {
        srcRect = walkingSrcRects[direction][frameIndex];
    } else {
        srcRect = idleSrcRect;
    }
}

void Player::update(float deltaTime) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (movingUp) moveY -= 1.0f;
    if (movingDown) moveY += 1.0f;
    if (movingLeft) moveX -= 1.0f;
    if (movingRight) moveX += 1.0f;

    // Normalize the movement vector if the player is moving diagonally
    if (moveX != 0.0f && moveY != 0.0f) {
        const float invLength = 1.0f / sqrt(moveX * moveX + moveY * moveY);
        moveX *= invLength;
        moveY *= invLength;
    }

    x += moveX * speed;
    y += moveY * speed;

    destRect.x = x;
    destRect.y = y;

    // Update the animation based on movement
    updateAnimation(deltaTime);
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
