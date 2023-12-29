#include "Map.h"
#include "../dep/FastNoiseLite.h"
#include <iostream>

Map::Map() {
    chunkSize = 32; // Example chunk size, adjust as needed
}

void Map::generateChunk(int chunkX, int chunkY, unsigned int seed) {
    std::cout << "Generating Chunk: " << chunkX << ", " << chunkY << std::endl;
    FastNoiseLite noise, riverNoise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetSeed(seed);

    riverNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    riverNoise.SetSeed(seed + 1);
    riverNoise.SetFrequency(0.02);

    Chunk newChunk;
    newChunk.tiles.reserve(chunkSize);

    std::vector<std::vector<TileType>> tempTypes(chunkSize, std::vector<TileType>(chunkSize));

    for (int y = 0; y < chunkSize; ++y) {
        for (int x = 0; x < chunkSize; ++x) {
            float biomeValue = noise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize));
            float riverValue = std::abs(riverNoise.GetNoise((float)(x + chunkX * chunkSize), (float)(y + chunkY * chunkSize)));
            TileType type = riverValue < 0.1 ? WATER : (biomeValue > 0 ? GRASS : WATER);
            tempTypes[y][x] = type;
        }
    }

    for (int y = 0; y < chunkSize; ++y) {
        std::vector<Tile> row;
        row.reserve(chunkSize);
        for (int x = 0; x < chunkSize; ++x) {
            TileType type = tempTypes[y][x];

            if (type == WATER) {
                // Check if adjacent to GRASS only (not SAND or WATER)
                bool adjacentToGrass = false;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < chunkSize && ny >= 0 && ny < chunkSize &&
                            tempTypes[ny][nx] == GRASS) {
                            adjacentToGrass = true;
                            break;
                        }
                    }
                    if (adjacentToGrass) break;
                }

                if (adjacentToGrass) {
                    // Change adjacent GRASS to SAND
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < chunkSize && ny >= 0 && ny < chunkSize &&
                                tempTypes[ny][nx] == GRASS) {
                                tempTypes[ny][nx] = SAND;
                            }
                        }
                    }
                }
            }

            row.emplace_back(tempTypes[y][x], (x + chunkX * chunkSize) * 32, (y + chunkY * chunkSize) * 32);
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
