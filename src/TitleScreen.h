#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <SDL.h>
#include <string>
#include <iostream>
#include "GameState.h"
#include "ui/UIManager.h"

class TitleScreen {
public:
    explicit TitleScreen(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    ~TitleScreen();
    void handleEvents(SDL_Event& event, GameState& gameState);
    void render();
    void handleWindowSizeChange(int newWidth, int newHeight);
    std::string getUIManagerSeedText() const;

private:
    SDL_Renderer* renderer;
    UIManager uiManager;
};

#endif
