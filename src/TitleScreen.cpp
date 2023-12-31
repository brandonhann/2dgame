#include "TitleScreen.h"
#include <ctime>
#include <chrono>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

TitleScreen::TitleScreen(SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : renderer(renderer), uiManager(renderer, windowWidth, windowHeight) {
}

TitleScreen::~TitleScreen() {
    // Destructor logic (if any)
}

void TitleScreen::handleEvents(SDL_Event& event, GameState& gameState) {
    uiManager.handleEvents(event);

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        // Check if click is on the play button
        // You'll need a method in UIManager to check this
        if (uiManager.isPlayButtonClicked(x, y)) {
            // Convert string to unsigned int for seed
            unsigned int newSeed = uiManager.getSeedText().empty() ? static_cast<unsigned int>(time(nullptr)) : std::stoul(uiManager.getSeedText());
            gameState = GameState::GAMEPLAY;
            // Pass newSeed to the Game class (not shown here)
        }
    }

    // Handle other global events like quitting the game
    if (event.type == SDL_QUIT) {
        gameState = GameState::QUIT;
    }
}

void TitleScreen::render() {
    uiManager.render();
    // Any additional rendering for TitleScreen
}

void TitleScreen::handleWindowSizeChange(int newWidth, int newHeight) {
    uiManager.handleWindowSizeChange(newWidth, newHeight);
}

std::string TitleScreen::getUIManagerSeedText() const {
    return uiManager.getSeedText();
}
