#include "UIManager.h"
#include <SDL_ttf.h>
#include <ctime>
#include <string>
#include <iostream>

UIManager::UIManager(SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : renderer(renderer), windowWidth(windowWidth), windowHeight(windowHeight), 
      isInputActive(false), caretPosition(0), caretVisible(true), lastCaretToggle(SDL_GetTicks()) {
    font = TTF_OpenFont(ROOT_PATH "assets/Fixedsys.ttf", 24);
    updateLayout();

    // Initialize seed text with a random value
    unsigned int initialSeed = static_cast<unsigned int>(time(nullptr));
    seedText = std::to_string(initialSeed);
}

void UIManager::updateLayout() {
    playButton.x = windowWidth / 2 - 50; // Centered and 100px wide
    playButton.y = windowHeight / 2 - 25; // Centered and 50px high
    playButton.w = 100;
    playButton.h = 50;

    inputField.x = windowWidth / 2 - 50; // Same horizontal position as play button
    inputField.y = playButton.y + 60; // 10px below the play button
    inputField.w = 100;
    inputField.h = 30;
}

void UIManager::handleEvents(SDL_Event& event) {
    updateCaretVisibility();

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        if (x >= inputField.x && x <= inputField.x + inputField.w &&
            y >= inputField.y && y <= inputField.y + inputField.h) {
            isInputActive = true;
            SDL_StartTextInput();
            caretPosition = calculateCaretPosition(x);
        } else {
            isInputActive = false;
            SDL_StopTextInput();
        }

        // More event handling if needed
    }

    if (isInputActive) {
        handleKeyboardInput(event);
    }
}

void UIManager::render() {
    SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
    SDL_RenderClear(renderer);

    SDL_Rect playButtonRect = {playButton.x, playButton.y, playButton.w, playButton.h};
    SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255);
    SDL_RenderFillRect(renderer, &playButtonRect);

    SDL_Rect playButtonInnerRect = {playButton.x + 2, playButton.y + 2, playButton.w - 4, playButton.h - 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playButtonInnerRect);

    SDL_Rect playButtonTextRect = {playButton.x, playButton.y, playButton.w, playButton.h};
    renderText("Play", ROOT_PATH "assets/Fixedsys.ttf", 24, playButtonTextRect, {0, 0, 0, 255});

    adjustInputFieldSize();

    SDL_Rect inputFieldRect = {inputField.x, inputField.y, inputField.w, inputField.h};
    SDL_SetRenderDrawColor(renderer, 130, 130, 130, 255);
    SDL_RenderDrawRect(renderer, &inputFieldRect);

    SDL_Rect inputFieldInnerRect = {inputField.x + 2, inputField.y + 2, inputField.w - 4, inputField.h - 4};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &inputFieldInnerRect);

    SDL_Rect seedTextBox = {inputField.x + 10, inputField.y + 5, inputField.w - 20, inputField.h - 10};
    renderText(seedText, ROOT_PATH "assets/Fixedsys.ttf", 24, seedTextBox, {0, 0, 0, 255});

    if (isInputActive && caretVisible) {
        std::string textUpToCaret = seedText.substr(0, caretPosition);
        int caretX, textHeight;
        TTF_SizeText(font, textUpToCaret.c_str(), &caretX, &textHeight);
        caretX += inputField.x + 10;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, caretX, inputField.y + 5, caretX, inputField.y + textHeight + 5);
    }

    SDL_RenderPresent(renderer);
}

void UIManager::handleWindowSizeChange(int newWidth, int newHeight) {
    windowWidth = newWidth;
    windowHeight = newHeight;
    updateLayout();
}

std::string UIManager::getSeedText() const {
    return seedText;
}

int UIManager::calculateCaretPosition(int mouseX) {
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

void UIManager::adjustInputFieldSize() {
    TTF_Font* font = TTF_OpenFont(ROOT_PATH "assets/Fixedsys.ttf", 24);
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

void UIManager::updateCaretVisibility() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastCaretToggle > caretToggleInterval) {
        caretVisible = !caretVisible;
        lastCaretToggle = currentTime;
    }
}

void UIManager::renderText(const std::string& text, const std::string& fontPath, int fontSize, SDL_Rect boundingBox, SDL_Color color) {
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

void UIManager::handleKeyboardInput(const SDL_Event& event) {
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

bool UIManager::isPlayButtonClicked(int x, int y) const {
    // Check if the x and y coordinates are within the play button's bounds
    return x >= playButton.x && x <= playButton.x + playButton.w &&
           y >= playButton.y && y <= playButton.y + playButton.h;
}
