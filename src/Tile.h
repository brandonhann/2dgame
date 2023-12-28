#ifndef TILE_H
#define TILE_H

#include <SDL.h>

enum TileType {
    GRASS,
    WATER,
    WALL
};

class Tile {
public:
    Tile(TileType type, int x, int y);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
    bool isWalkable();

private:
    TileType type;
    SDL_Rect srcRect, destRect;
};

#endif
