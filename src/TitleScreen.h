#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <SDL.h>
#include "GameState.h"
#include <string>

class TitleScreen {
public:
    explicit TitleScreen(SDL_Renderer* renderer);
    ~TitleScreen();
    void renderSeedText(SDL_Renderer* renderer, const std::string& text, int x, int y);
    void handleEvents(SDL_Event& event, GameState& gameState);
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Rect playButton;
    SDL_Rect inputField;
    std::string seedText;
    bool isInputActive;
};

#endif
