#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <SDL.h>
#include <string>
#include <iostream>
#include "GameState.h"
#include "SDL_ttf.h"

class TitleScreen {
public:
    explicit TitleScreen(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    ~TitleScreen();
    void handleEvents(SDL_Event& event, GameState& gameState);
    void render();
    void handleWindowSizeChange(int newWidth, int newHeight);
    std::string getSeedText() const;

private:
    SDL_Renderer* renderer;
    SDL_Rect playButton; // Rectangle for the play button
    SDL_Rect inputField; // Rectangle for the input field
    std::string seedText; // Text for the seed input
    bool isInputActive; // Flag to indicate if input field is active
    int windowWidth; // Current width of the window
    int windowHeight; // Current height of the window
    void updateLayout();
    void adjustInputFieldSize(); // Declaration of the adjustInputFieldSize method
    void renderText(const std::string& text, const std::string& fontPath, int fontSize, SDL_Rect boundingBox, SDL_Color color);
    int caretPosition; // Position of the caret in the seed text
    bool caretVisible; // Whether the caret is currently visible
    Uint32 lastCaretToggle; // Time since the last caret toggle
    const int caretToggleInterval = 500; // Caret blink interval in milliseconds
    void updateCaretVisibility();
    TTF_Font* font;
    int calculateCaretPosition(int mouseX);
    void handleKeyboardInput(const SDL_Event& event);
};

#endif
