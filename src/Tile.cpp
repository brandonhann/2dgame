#include "Tile.h"
#include <SDL_image.h> // Make sure to include SDL_image for texture loading
#include <iostream>

// Static member for the texture
SDL_Texture* Tile::tilesetTexture = nullptr;

// Constructor remains the same
Tile::Tile(TileType type, int x, int y) : type(type) {
    srcRect = {0, 0, 32, 32}; // Size of the texture frame
    destRect = {x, y, 32, 32}; // Size and position on the screen
}

// Static method to load the tileset texture
void Tile::loadTilesetTexture(SDL_Renderer* renderer, const char* filePath) {
    // Load the texture from a file
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, filePath);
    if(newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return;
    }
    // If a texture is already loaded, destroy it before replacing
    if(Tile::tilesetTexture != nullptr) {
        SDL_DestroyTexture(Tile::tilesetTexture);
    }
    Tile::tilesetTexture = newTexture;
}

// TODO generate properites for these types, maybe keep it in a JSON file. (example: WALKSPEED, COLLIDE, FLOAT, SLIP)

// Render method now uses the texture
void Tile::render(SDL_Renderer* renderer, SDL_Rect& camera) {
    SDL_Rect renderQuad = {destRect.x - camera.x, destRect.y - camera.y, destRect.w, destRect.h};
    
    // Here you'll set srcRect to the correct texture position based on the tile type
    switch(type) {
        case GRASS:
            srcRect.x = 0;
            srcRect.y = 0;
            break;
        case WATER:
            srcRect.x = 32;
            srcRect.y = 0;
            break;
        case SAND:
            srcRect.x = 64;
            srcRect.y = 0;
            break;
        case SNOW:
            srcRect.x = 96;
            srcRect.y = 0;
            break;
        // Add additional cases for other tile types
        // ...
    }

    // Render the part of the texture to the screen
    SDL_RenderCopy(renderer, Tile::tilesetTexture, &srcRect, &renderQuad);
}
