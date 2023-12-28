#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include "Player.h"
#include "Map.h"

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

private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect camera;
    Player *player;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    Map gameMap;
    int chunkSize;
    unsigned int seed;
};

#endif
