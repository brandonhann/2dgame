#include "Tile.h"
#include <SDL_image.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// Static member initialization
SDL_Texture* Tile::tilesetTexture = nullptr;
json Tile::tileProperties = json();

// Constructor
Tile::Tile(TileType type, int x, int y) : type(type) {
    destRect = {x, y, 32, 32}; // Size and position on the screen
    setPropertiesFromJson();
}

// Load the tileset texture
void Tile::loadTilesetTexture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, filePath);
    if (newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << SDL_GetError() << std::endl;
        return;
    }
    if (Tile::tilesetTexture != nullptr) {
        SDL_DestroyTexture(Tile::tilesetTexture);
    }
    Tile::tilesetTexture = newTexture;
}

// Load and parse the JSON file for tile properties
void Tile::loadTileProperties(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        file >> Tile::tileProperties;
    } else {
        std::cerr << "Failed to open tile properties file: " << filePath << std::endl;
    }
}

// Free the tileset texture
void Tile::freeTilesetTexture() {
    if (Tile::tilesetTexture != nullptr) {
        SDL_DestroyTexture(Tile::tilesetTexture);
        tilesetTexture = nullptr;
    }
}

// Set properties based on JSON data
void Tile::setPropertiesFromJson() {
    std::string tileTypeName = getTileTypeName();
    if (!Tile::tileProperties.contains(tileTypeName)) {
        std::cerr << "Tile type not found in JSON: " << tileTypeName << std::endl;
        return;
    }

    json props = Tile::tileProperties[tileTypeName];
    
    // Set srcRect from JSON
    if (props.contains("srcRect") && props["srcRect"].is_object()) {
        json srcRectJson = props["srcRect"];
        srcRect.x = srcRectJson.value("x", 0);
        srcRect.y = srcRectJson.value("y", 0);
        srcRect.w = srcRect.h = 32; // Assuming each tile is 32x32
    } else {
        std::cerr << "srcRect not found or invalid for tile type: " << tileTypeName << std::endl;
    }

    // Set other properties here as needed
    // Example: 
    // bool isWater = props.value("isWater", false);
}

// Convert TileType enum to string for JSON key
std::string Tile::getTileTypeName() {
    switch (type) {
        case GRASS: return "GRASS";
        case WATER: return "WATER";
        case SAND: return "SAND";
        case SNOW: return "SNOW";
        // ... Add cases for other tile types
        default: return "UNKNOWN";
    }
}

// Render method
void Tile::render(SDL_Renderer* renderer, SDL_Rect& camera) {
    SDL_Rect renderQuad = {destRect.x - camera.x, destRect.y - camera.y, destRect.w, destRect.h};
    SDL_RenderCopy(renderer, Tile::tilesetTexture, &srcRect, &renderQuad);
}
