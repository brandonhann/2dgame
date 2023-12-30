#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <SDL.h>
#include "GameState.h"

class TitleScreen {
public:
    explicit TitleScreen(SDL_Renderer* renderer);
    ~TitleScreen();

    void handleEvents(SDL_Event& event, GameState& gameState);
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Rect playButton; // Rectangle for the play button
};

#endif
