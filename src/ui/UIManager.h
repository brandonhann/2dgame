#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SDL.h>
#include <string>
#include <SDL_ttf.h>

class UIManager {
public:
    UIManager(SDL_Renderer* renderer, int windowWidth, int windowHeight);
    ~UIManager();
    void handleEvents(SDL_Event& event);
    void render();
    void handleWindowSizeChange(int newWidth, int newHeight);
    std::string getSeedText() const;
    void updateLayout();
    bool isPlayButtonClicked(int x, int y) const;

private:
    SDL_Renderer* renderer;
    SDL_Rect playButton; // Rectangle for the play button
    SDL_Rect inputField; // Rectangle for the input field
    std::string seedText; // Text for the seed input
    bool isInputActive; // Flag to indicate if input field is active
    int windowWidth; // Current width of the window
    int windowHeight; // Current height of the window
    void adjustInputFieldSize(); // Adjust the size of the input field
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
