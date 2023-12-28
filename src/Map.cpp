#include "Map.h"
#include "../dep/FastNoiseLite.h"
#include <iostream>

Map::Map() {
    chunkSize = 32; // Example chunk size, adjust as needed
}

void Map::generateChunk(int chunkX, int chunkY, unsigned int seed) {
    std::cout << "Generating Chunk: " << chunkX << ", " << chunkY << std::endl;
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetSeed(seed);

    Chunk newChunk;
    newChunk.tiles.reserve(chunkSize);

    for (int y = 0; y < chunkSize; ++y) {
        std::vector<Tile> row;
        row.reserve(chunkSize);
        for (int x = 0; x < chunkSize; ++x) {
            float noiseValue = noise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize));
            TileType type = noiseValue > 0 ? GRASS : WATER;
            row.emplace_back(type, (x + chunkX * chunkSize) * 32, (y + chunkY * chunkSize) * 32);
        }
        newChunk.tiles.push_back(std::move(row));
    }

    chunks[std::make_pair(chunkX, chunkY)] = std::move(newChunk);
}

void Map::render(SDL_Renderer* renderer, SDL_Rect& camera) {
    // Determine visible chunks based on camera position
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
                        tile.render(renderer, camera);
                    }
                }
            }
        }
    }
}

bool Map::isChunkGenerated(int chunkX, int chunkY) {
    return chunks.find(std::make_pair(chunkX, chunkY)) != chunks.end();
}
