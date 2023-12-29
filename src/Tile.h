#ifndef TILE_H
#define TILE_H

#include <SDL.h>

enum TileType {
    GRASS,
    WATER,
    WALL,
    SAND,
    SNOW
};

class Tile {
public:
    Tile(TileType type, int x, int y);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
    bool isWalkable();
    TileType getType() const { return type; }
    static void setTilesetTexture(SDL_Texture* newTexture);
     static void loadTilesetTexture(SDL_Renderer* renderer, const char* filePath);
    static void freeTilesetTexture() {
        if(tilesetTexture != nullptr) {
            SDL_DestroyTexture(tilesetTexture);
            tilesetTexture = nullptr;
        }
    }

private:
    TileType type;
    SDL_Rect srcRect, destRect;
    static SDL_Texture* tilesetTexture;
};

#endif
