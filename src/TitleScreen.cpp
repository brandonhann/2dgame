#include "TitleScreen.h"
#include <ctime>
#include <chrono>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

TitleScreen::TitleScreen(SDL_Renderer* renderer) : renderer(renderer), isInputActive(false) {
    // Define the play button dimensions and position
    playButton.x = 350; // Example position, adjust as needed
    playButton.y = 300; // Example position, adjust as needed
    playButton.w = 100; // Example width
    playButton.h = 50;  // Example height

    // Initialize input field rectangle (position below the play button)
    inputField.x = 350; // Adjust as necessary
    inputField.y = 360; // Below the play button
    inputField.w = 100;
    inputField.h = 30;

    // Initialize seed text with a random value
    unsigned int initialSeed = static_cast<unsigned int>(time(nullptr));
    seedText = std::to_string(initialSeed);
}

TitleScreen::~TitleScreen() {
    // Free resources if any are used
}

void TitleScreen::handleEvents(SDL_Event& event, GameState& gameState) {
    // Handle mouse click for input field
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        // Check if click is inside the input field
        if (x >= inputField.x && x <= inputField.x + inputField.w &&
            y >= inputField.y && y <= inputField.y + inputField.h) {
            isInputActive = true;
        }
        // Check if click is on the play button
        else if (x >= playButton.x && x <= playButton.x + playButton.w &&
                 y >= playButton.y && y <= playButton.y + playButton.h) {
            unsigned int newSeed = std::stoul(seedText); // Convert string to unsigned int
            gameState = GameState::GAMEPLAY;
            // Pass newSeed to the Game class (not shown here)
        }
    }

    // Handle keyboard input for seed text
    if (isInputActive && event.type == SDL_KEYDOWN) {
        // Append character to seedText or handle backspace/delete
        // This is a simplified example; you'll need to handle special keys, etc.
        if (event.key.keysym.sym == SDLK_BACKSPACE && !seedText.empty()) {
            seedText.pop_back();
        } else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) {
            seedText += static_cast<char>(event.key.keysym.sym);
        }
    }
}

void TitleScreen::render() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Render the play button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for the button
    SDL_RenderFillRect(renderer, &playButton);

    // Render the input field for the seed
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for the input field outline
    SDL_RenderDrawRect(renderer, &inputField);

    // Render the seed text
    renderSeedText(renderer, seedText, inputField.x + 10, inputField.y + 5); // Adjust text position as needed

    // Update the screen
    SDL_RenderPresent(renderer);
}

void TitleScreen::renderSeedText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    // Create an SDL_Surface with the text
    SDL_Color textColor = {0, 0, 0, 255}; // Black color for the text
    TTF_Font* font = TTF_OpenFont("assets/Fixedsys.ttf", 24); // Specify your font and font size
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == nullptr) {
        TTF_CloseFont(font);
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface and render it
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
    } else {
        SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
        SDL_DestroyTexture(textTexture);
    }

    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}
