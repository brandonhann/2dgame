#include "TitleScreen.h"
#include <ctime>
#include <chrono>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

TitleScreen::TitleScreen(SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : renderer(renderer), windowWidth(windowWidth), windowHeight(windowHeight), 
      isInputActive(false), caretPosition(0), caretVisible(true), lastCaretToggle(SDL_GetTicks()) {
    font = TTF_OpenFont("assets/Fixedsys.ttf", 24);
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

    // Initialize UI elements
    updateLayout();
    // Initialize seed text with a random value
    unsigned int initialSeed = static_cast<unsigned int>(time(nullptr));
    seedText = std::to_string(initialSeed);
}

TitleScreen::~TitleScreen() {
    if (font) {
        TTF_CloseFont(font); // Free the font resource
    }
}

void TitleScreen::updateLayout() {
    // Define the play button dimensions and position relative to window size
    playButton.x = windowWidth / 2 - 50; // Centered and 100px wide
    playButton.y = windowHeight / 2 - 25; // Centered and 50px high
    playButton.w = 100;
    playButton.h = 50;

    // Initialize input field rectangle below the play button
    inputField.x = windowWidth / 2 - 50; // Same horizontal position as play button
    inputField.y = playButton.y + 60; // 10px below the play button
    inputField.w = 100;
    inputField.h = 30;
}

void TitleScreen::handleWindowSizeChange(int newWidth, int newHeight) {
    windowWidth = newWidth;
    windowHeight = newHeight;
    updateLayout();
}

void TitleScreen::handleEvents(SDL_Event& event, GameState& gameState) {
    // Update caret visibility for blinking effect
    updateCaretVisibility();

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        // Check if click is inside the input field
        if (x >= inputField.x && x <= inputField.x + inputField.w &&
            y >= inputField.y && y <= inputField.y + inputField.h) {
            isInputActive = true;
            SDL_StartTextInput();
            // Calculate caret position based on click location
            caretPosition = calculateCaretPosition(x);
        } else {
            isInputActive = false;
            SDL_StopTextInput();
        }

        // Check if click is on the play button
        if (x >= playButton.x && x <= playButton.x + playButton.w &&
            y >= playButton.y && y <= playButton.y + playButton.h) {
            // Convert string to unsigned int for seed
            unsigned int newSeed = seedText.empty() ? static_cast<unsigned int>(time(nullptr)) : std::stoul(seedText);
            gameState = GameState::GAMEPLAY;
            // Pass newSeed to the Game class (not shown here)
        }
    }

    if (isInputActive) {
        if (event.type == SDL_TEXTINPUT) {
            seedText.insert(caretPosition, event.text.text);
            caretPosition += strlen(event.text.text);
            adjustInputFieldSize();
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_BACKSPACE && caretPosition > 0) {
                seedText.erase(caretPosition - 1, 1);
                caretPosition--;
                adjustInputFieldSize();
            } else if (event.key.keysym.sym == SDLK_DELETE && caretPosition < static_cast<int>(seedText.length())) {
                seedText.erase(caretPosition, 1);
                adjustInputFieldSize();
            } else if (event.key.keysym.sym == SDLK_LEFT && caretPosition > 0) {
                caretPosition--;
            } else if (event.key.keysym.sym == SDLK_RIGHT && caretPosition < static_cast<int>(seedText.length())) {
                caretPosition++;
            }
        }
    }

    // Handle other global events like quitting the game
    if (event.type == SDL_QUIT) {
        gameState = GameState::QUIT;
    }
}

std::string TitleScreen::getSeedText() const {
    return seedText;
}

int TitleScreen::calculateCaretPosition(int mouseX) {
    // Calculate the starting X position of the text
    int startX = inputField.x + 10; // Assuming 10 pixels padding

    // Calculate the width of a single character
    int charWidth, charHeight;
    TTF_SizeText(font, "A", &charWidth, &charHeight); // Use a representative character

    // Calculate relative position of the click from the start of the text
    int relativeX = mouseX - startX;

    // Calculate the estimated caret position
    int estimatedPosition = relativeX / charWidth;

    // Clamp the estimatedPosition to the length of the seedText
    estimatedPosition = std::min(estimatedPosition, static_cast<int>(seedText.length()));
    estimatedPosition = std::max(estimatedPosition, 0);

    return estimatedPosition;
}

void TitleScreen::handleKeyboardInput(const SDL_Event& event) {
    if (event.key.keysym.sym == SDLK_BACKSPACE && caretPosition > 0) {
        seedText.erase(caretPosition - 1, 1);
        caretPosition--;
        adjustInputFieldSize();
    } else if (event.key.keysym.sym == SDLK_DELETE && caretPosition < static_cast<int>(seedText.length())) {
        seedText.erase(caretPosition, 1);
        adjustInputFieldSize();
    } else if (event.key.keysym.sym == SDLK_LEFT && caretPosition > 0) {
        caretPosition--;
    } else if (event.key.keysym.sym == SDLK_RIGHT && caretPosition < static_cast<int>(seedText.length())) {
        caretPosition++;
    }
}

void TitleScreen::adjustInputFieldSize() {
    TTF_Font* font = TTF_OpenFont("assets/Fixedsys.ttf", 24);
    if (font != nullptr) {
        int textWidth, textHeight;
        TTF_SizeText(font, seedText.c_str(), &textWidth, &textHeight);

        // Update input field width to fit the text, with some padding
        inputField.w = std::max(100, textWidth + 20); // Minimum width is 100

        // Center the input field based on the new width
        inputField.x = windowWidth / 2 - inputField.w / 2;

        TTF_CloseFont(font);
    }
}

void TitleScreen::updateCaretVisibility() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCaretToggle > caretToggleInterval) {
        caretVisible = !caretVisible;
        lastCaretToggle = currentTime;
    }
}

void TitleScreen::render() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255); // Light grey background
    SDL_RenderClear(renderer);

    // Draw the play button
    SDL_Rect playButtonRect = {playButton.x, playButton.y, playButton.w, playButton.h};
    SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255); // Dark grey for button shadow
    SDL_RenderFillRect(renderer, &playButtonRect);

    SDL_Rect playButtonInnerRect = {playButton.x + 2, playButton.y + 2, playButton.w - 4, playButton.h - 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for button face
    SDL_RenderFillRect(renderer, &playButtonInnerRect);

    // Render the text "Play" onto the button
    SDL_Rect playButtonTextRect = {playButton.x, playButton.y, playButton.w, playButton.h};
    renderText("Play", "assets/Fixedsys.ttf", 24, playButtonTextRect, {0, 0, 0, 255});

    // Adjust the input field size based on the seed text length
    adjustInputFieldSize();

    // Render the input field for the seed
    SDL_Rect inputFieldRect = {inputField.x, inputField.y, inputField.w, inputField.h};
    SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255); // Dark grey for input field shadow
    SDL_RenderDrawRect(renderer, &inputFieldRect);

    SDL_Rect inputFieldInnerRect = {inputField.x + 2, inputField.y + 2, inputField.w - 4, inputField.h - 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for input field face
    SDL_RenderFillRect(renderer, &inputFieldInnerRect);

    // Render the seed text inside the input field
    SDL_Rect seedTextBox = {inputField.x + 10, inputField.y + 5, inputField.w - 20, inputField.h - 10};
    renderText(seedText, "assets/Fixedsys.ttf", 24, seedTextBox, {0, 0, 0, 255});

    // Render the caret if input field is active
    if (isInputActive && caretVisible) {
        std::string textUpToCaret = seedText.substr(0, caretPosition);
        int caretX, textHeight;
        TTF_SizeText(font, textUpToCaret.c_str(), &caretX, &textHeight);
        caretX += inputField.x + 10; // Adjust position based on input field

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color for caret
        SDL_RenderDrawLine(renderer, caretX, inputField.y + 5, caretX, inputField.y + textHeight + 5);
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}

void TitleScreen::renderText(const std::string& text, const std::string& fontPath, int fontSize, SDL_Rect boundingBox, SDL_Color color) {
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr) {
        TTF_CloseFont(font);
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }

    // Center the text within the bounding box
    int textX = boundingBox.x + (boundingBox.w - surface->w) / 2;
    int textY = boundingBox.y + (boundingBox.h - surface->h) / 2;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect renderQuad = {textX, textY, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &renderQuad);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
}
