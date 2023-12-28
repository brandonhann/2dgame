#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

class Player {
public:
    Player(int x, int y);
    int getX() const;
    int getY() const;
    void update();
    void render(SDL_Renderer* renderer, const SDL_Rect& camera);
    void setMovingUp(bool move);
    void setMovingDown(bool move);
    void setMovingLeft(bool move);
    void setMovingRight(bool move);

private:
    int x, y, speed;
    bool movingUp, movingDown, movingLeft, movingRight;
    SDL_Rect srcRect, destRect;
};

#endif
