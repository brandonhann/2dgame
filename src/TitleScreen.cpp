#include "TitleScreen.h"
#include <ctime> // For random seed

TitleScreen::TitleScreen(SDL_Renderer* renderer) : renderer(renderer) {
    // Define the play button dimensions and position
    playButton.x = 350; // Example position, adjust as needed
    playButton.y = 300; // Example position, adjust as needed
    playButton.w = 100; // Example width
    playButton.h = 50;  // Example height

    // Optionally load a texture for the button or use text rendering
}

TitleScreen::~TitleScreen() {
    // Free resources if any are used
}

#include <chrono> // Include the chrono header for high-resolution clock

void TitleScreen::handleEvents(SDL_Event& event, GameState& gameState) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        if (x >= playButton.x && x <= playButton.x + playButton.w &&
            y >= playButton.y && y <= playButton.y + playButton.h) {
            gameState = GameState::GAMEPLAY; // Change game state to gameplay
        }
    }
}

void TitleScreen::render() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Render the title screen assets
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for the button
    SDL_RenderFillRect(renderer, &playButton); // Draw the play button

    // Update the screen
    SDL_RenderPresent(renderer);
}
