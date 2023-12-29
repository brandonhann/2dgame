#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>

class Player {
public:
    Player(int x, int y);
    int getX() const;
    int getY() const;
    void update(float deltaTime); // Updated to include deltaTime
    void render(SDL_Renderer* renderer, const SDL_Rect& camera);
    void handleInput(const SDL_Event& event);
    void setMovingUp(bool move);
    void setMovingDown(bool move);
    void setMovingLeft(bool move);
    void setMovingRight(bool move);

private:
    int x, y, speed;
    bool movingUp, movingDown, movingLeft, movingRight;
    SDL_Rect srcRect, destRect;
    std::string currentBiome; // New member
    std::string lastBiome; // New member
    float timeSinceLastBiomeChange; // New member
    static const float BIOME_CHANGE_COOLDOWN; // New member
};

#endif
