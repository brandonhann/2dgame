#ifndef MAP_H
#define MAP_H

#include "Tile.h"
#include <vector>
#include <map>
#include <utility> // For std::pair

class Chunk {
public:
    std::vector<std::vector<Tile>> tiles;
};

class Map {
public:
    Map();
    bool isChunkGenerated(int chunkX, int chunkY);
    void generateChunk(int chunkX, int chunkY, unsigned int seed);
    void render(SDL_Renderer* renderer, SDL_Rect& camera);
    std::map<std::pair<int, int>, Chunk> chunks; 

private:
    int chunkSize; // Size of each chunk in tiles
};

#endif
