#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>
#include <array>

class Player {
public:
    Player(int x, int y);
    int getX() const;
    int getY() const;
    void update(float deltaTime);
    void render(SDL_Renderer* renderer, const SDL_Rect& camera);
    void handleInput(const SDL_Event& event);
    static void loadPlayerTexture(SDL_Renderer* renderer, const char* filePath);
    static void destroyTexture();

    // Setters for movement states
    void setMovingUp(bool move);
    void setMovingDown(bool move);
    void setMovingLeft(bool move);
    void setMovingRight(bool move);
     void setWalkSpeed(int newSpeed);

private:
    int x, y, speed;
    bool movingUp, movingDown, movingLeft, movingRight;
    SDL_Rect srcRect, destRect;
    std::string currentBiome; 
    std::string lastBiome;
    float timeSinceLastBiomeChange;
    static const float BIOME_CHANGE_COOLDOWN;
    static SDL_Texture* playerTexture;

    // Animation related members
    int frameIndex; // Current frame in the animation
    float frameTime; // Time since last frame change
    float animationSpeed; // How long each frame is displayed
    static const int numFrames = 2; // Number of frames per animation
    static const int numDirections = 4; // Number of directions (down, left, up, right)

    // Arrays to hold the source rectangles for each animation frame
    SDL_Rect idleSrcRect; // Source rectangle for idle frame
    std::array<std::array<SDL_Rect, numFrames>, numDirections> walkingSrcRects; // Source rectangles for walking animations

    // Helper function to update animation
    void updateAnimation(float deltaTime);
};

#endif
