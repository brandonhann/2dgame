#include "Map.h"
#include "../dep/FastNoiseLite.h"
#include <iostream>

const int Map::numberOfChunksWidth = 100;  // Example value for map width
const int Map::numberOfChunksHeight = 100; // Example value for map height

Map::Map(unsigned int seed) : seed(seed), chunkSize(32),
    grasslandThreshold(-0.2), // Adjust this for more Grassland
    snowThreshold(-0.6) {     // Adjust this for more Snow
    // Noise setup
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    riverNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    // Adjusting noise parameters
    noise.SetSeed(seed);
    biomeNoise.SetSeed(seed + 1);
    riverNoise.SetSeed(seed + 2);
    riverNoise.SetFrequency(0.05);

    loadTileProperties();
}

void Map::loadTileProperties() {
    Tile::loadTileProperties(ROOT_PATH "src/tile_props.json");
}

TileType Map::generateGrasslandTile(float noiseValue, float riverNoiseValue, float biomeValue, int x, int y) {
    // Example implementation for grassland
    // Adjust thresholds and logic as per your game's design

    const float riverThreshold = 0.2; // Threshold for rivers
    const float riverEdgeThreshold = 0.25; // Threshold for river edges

    bool isRiver = riverNoiseValue < riverThreshold;
    bool isRiverEdge = riverNoiseValue >= riverThreshold && riverNoiseValue < riverEdgeThreshold;

    if (isRiver) {
        return WATER; // River tile
    } else if (isRiverEdge) {
        return SAND; // River edge, possibly a beach
    } else {
        return GRASS; // Grassland
    }
}

TileType Map::generateSnowTile(float noiseValue, float biomeValue, int x, int y) {
    // Simple implementation for snow/tundra biome
    // You can add more complex logic for features like frozen lakes

    return SNOW; // Snow tile
}

void Map::generateChunk(int chunkX, int chunkY, unsigned int seed) {
    // Initialize the new chunk
    Chunk newChunk;
    newChunk.tiles.reserve(chunkSize);

    // Temporary storage for tile types before finalizing the chunk
    std::vector<std::vector<TileType>> tempTypes(chunkSize, std::vector<TileType>(chunkSize));

    // First pass: Generate basic terrain types (grass and snow)
    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            // Note: Using class-level noise objects
            float biomeValue = this->biomeNoise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize));
            float noiseValue = this->noise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize));
            float riverNoiseValue = std::abs(this->riverNoise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize)));

            TileType type;
            if (biomeValue > grasslandThreshold) {
                type = generateGrasslandTile(noiseValue, riverNoiseValue, biomeValue, x, y);
            } else {
                type = generateSnowTile(noiseValue, biomeValue, x, y);
            }

            tempTypes[y][x] = type;
        }
    }

    // Second pass: Adjust for beaches (sand) near water bodies
    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            if (tempTypes[y][x] == GRASS && checkAdjacentToWater(x, y, tempTypes)) {
                tempTypes[y][x] = SAND;
            }
        }
    }

    // Finalize the chunk with the determined tile types
    for (int y = 0; y < chunkSize; ++y) {
        std::vector<Tile> row;
        row.reserve(chunkSize);
        for (int x = 0; x < chunkSize; ++x) {
            TileType type = tempTypes[y][x];
            row.emplace_back(type, (x + chunkX * chunkSize) * 32, (y + chunkY * chunkSize) * 32);
        }
        newChunk.tiles.push_back(std::move(row));
    }

    // Store the newly generated chunk
    chunks[std::make_pair(chunkX, chunkY)] = std::move(newChunk);
}

bool Map::checkAdjacentToWater(int x, int y, const std::vector<std::vector<TileType>>& tempTypes) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip the current tile

            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= chunkSize || ny >= chunkSize) continue; // Check bounds

            if (tempTypes[ny][nx] == WATER) {
                return true;
            }
        }
    }
    return false;
}

TileType Map::getTileAt(int x, int y) {
    int chunkX = x / chunkSize;
    int chunkY = y / chunkSize;
    int tileX = x % chunkSize;
    int tileY = y % chunkSize;

    auto chunkIt = chunks.find(std::make_pair(chunkX, chunkY));
    if (chunkIt != chunks.end()) {
        Chunk &chunk = chunkIt->second;
        return chunk.tiles[tileY][tileX].getType();
    } else {
        return WATER; // Or some other default type
    }
}

void Map::render(SDL_Renderer* renderer, SDL_Rect& camera) {
    int startChunkX = std::floor(static_cast<float>(camera.x) / (chunkSize * 32));
    int startChunkY = std::floor(static_cast<float>(camera.y) / (chunkSize * 32));
    int endChunkX = std::ceil(static_cast<float>(camera.x + camera.w) / (chunkSize * 32));
    int endChunkY = std::ceil(static_cast<float>(camera.y + camera.h) / (chunkSize * 32));

    for (int chunkY = startChunkY; chunkY < endChunkY; ++chunkY) {
        for (int chunkX = startChunkX; chunkX < endChunkX; ++chunkX) {
            auto it = chunks.find(std::make_pair(chunkX, chunkY));
            if (it != chunks.end()) {
                Chunk& chunk = it->second;
                for (auto& row : chunk.tiles) {
                    for (auto& tile : row) {
                        tile.render(renderer, camera); // Assuming Tile class has a render method
                    }
                }
            }
        }
    }
}

void Map::removeOutOfViewChunks(int visibleStartX, int visibleEndX, int visibleStartY, int visibleEndY) {
    auto it = chunks.begin();
    while (it != chunks.end()) {
        int chunkX = it->first.first;
        int chunkY = it->first.second;
        if (chunkX < visibleStartX || chunkX > visibleEndX || chunkY < visibleStartY || chunkY > visibleEndY) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

bool Map::isChunkGenerated(int chunkX, int chunkY) const {
    auto chunkIt = chunks.find(std::make_pair(chunkX, chunkY));
    return chunkIt != chunks.end();
}
