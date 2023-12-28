#include "Tile.h"


Tile::Tile(TileType type, int x, int y) : type(type) {
    srcRect = {0, 0, 32, 32}; // Size of the texture frame
    destRect = {x, y, 32, 32}; // Size and position on the screen
}

void Tile::render(SDL_Renderer* renderer, SDL_Rect& camera) {
    SDL_Rect renderQuad = {destRect.x - camera.x, destRect.y - camera.y, destRect.w, destRect.h};

    // Set color based on tile type (for example)
    switch(type) {
        case GRASS:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for grass
            break;
        case WATER:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for water
            break;
        case WALL:
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Grey for walls
            break;
        // Add cases for other tile types if necessary
    }

    SDL_RenderFillRect(renderer, &renderQuad);
}

bool Tile::isWalkable() {
    // Implement logic to determine if a tile is walkable
    // For example, assuming WALL is not walkable
    return type != WALL;
}
