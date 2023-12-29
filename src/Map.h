#ifndef MAP_H
#define MAP_H

#include "Tile.h"
#include "../dep/FastNoiseLite.h"
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <SDL.h> // Include SDL for rendering

class Chunk {
public:
    std::vector<std::vector<Tile>> tiles;
};

class Map {
public:
    Map(unsigned int seed);
    void generateChunk(int chunkX, int chunkY, unsigned int seed);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
    void removeOutOfViewChunks(int visibleStartX, int visibleEndX, int visibleStartY, int visibleEndY);
    bool isChunkGenerated(int chunkX, int chunkY) const;
    std::string getBiomeAt(int x, int y);
    TileType getTileAt(int x, int y);

    static const int numberOfChunksWidth; // Define these based on your map's size
    static const int numberOfChunksHeight;

private:
    TileType generateGrasslandTile(float noiseValue, float riverNoiseValue, float biomeValue, int x, int y);
    TileType generateSnowTile(float noiseValue, float biomeValue, int x, int y);

    bool checkAdjacentToWater(int x, int y, const std::vector<std::vector<TileType>>& tempTypes);

    unsigned int seed;
    int chunkSize;
    std::map<std::pair<int, int>, Chunk> chunks;
    float grasslandThreshold;
    float snowThreshold;
    FastNoiseLite noise, biomeNoise, riverNoise;
};

#endif
