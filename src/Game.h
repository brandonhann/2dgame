#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "GameState.h"
#include "Player.h"
#include "Map.h"

class TitleScreen;  // Forward declaration of TitleScreen

class Game {
public:
    Game();
    ~Game();
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    void updateCamera();
    bool running();
    void setGameState(GameState newState);
    void setSeed(unsigned int newSeed);

private:
    GameState gameState;
    TitleScreen* titleScreen;
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect camera;
    Player* player;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    Map gameMap;  // Single declaration of gameMap
    int chunkSize;
    unsigned int seed;
    bool seedNeedsUpdate;
    bool displaySeedMessage;
    Uint32 seedMessageStartTime;
    const Uint32 seedMessageDuration = 5000; // 5 seconds
};

#endif
