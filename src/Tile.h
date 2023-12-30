#ifndef TILE_H
#define TILE_H

#include <SDL.h>

enum TileType {
    GRASS,
    WATER,
    SAND,
    SNOW,
    DEEP_WATER,
    MUD,
    ICE,
    SNOWY_GRASS,
    SNOWY_SAND,
    SNOWY_MUD,
};

class Tile {
public:
    Tile(TileType type, int x, int y);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
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
