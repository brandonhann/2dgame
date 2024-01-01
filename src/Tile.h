#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <nlohmann/json.hpp>
#include <string>

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
    // Add other tile types as needed
};

class Tile {
public:
    Tile(TileType type, int x, int y);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
    TileType getType() const { return type; }
    static void loadTilesetTexture(SDL_Renderer* renderer, const char* filePath);
    static void loadTileProperties(const std::string& filePath);
    static void freeTilesetTexture();

private:
    TileType type;
    SDL_Rect srcRect, destRect;
    static SDL_Texture* tilesetTexture;
    static nlohmann::json tileProperties;

    void setTileRect();
    void setPropertiesFromJson();
    std::string getTileTypeName();
};

#endif
