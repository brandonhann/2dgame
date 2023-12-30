#include "Game.h"
#include "Player.h"
#include <iostream>
#include <SDL_image.h>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), chunkSize(32), seed(12345), gameMap(seed) {
    player = new Player(100, 100); // Initialize player with a position
    camera = {0, 0, 800, 600};

    int initialChunkX = player->getX() / (32 * chunkSize);
    int initialChunkY = player->getY() / (32 * chunkSize);

    for (int y = initialChunkY - 1; y <= initialChunkY + 1; y++) {
        for (int x = initialChunkX - 1; x <= initialChunkX + 1; x++) {
            gameMap.generateChunk(x, y, seed);
        }
    }
}

Game::~Game() {
    delete player; // Delete the player object
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = SDL_WINDOW_RESIZABLE; // Add resizable flag
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        // Create window
        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        
        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer) {
            // Initialize SDL_image for image loading
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
                std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
            } else {
                // Load the tileset texture for the Tile class
                Tile::loadTilesetTexture(renderer, "assets/tilemap.png");
                // Load the player texture
                Player::loadPlayerTexture(renderer, "assets/player_sprite_map.png"); // This is the new line to add
            }

            // Set draw color for renderer to white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
        }
        isRunning = true;
    } else {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                player->handleInput(event); // Delegate to player
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Update the camera size with the new window dimensions
                    camera.w = event.window.data1;
                    camera.h = event.window.data2;
                    // Optionally, re-adjust any other relevant game properties or layout here
                }
                break;
        }
    }
}

void Game::updateCamera() {
    // Center the camera over the player
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;
}

void Game::update() {
    // Get the time at the start of the frame
    Uint32 frameStart = SDL_GetTicks();

    // Function to calculate the chunk index for a given coordinate
    auto getChunkIndex = [this](int coordinate) -> int {
        return coordinate / (32 * chunkSize);
    };

    float deltaTime = (SDL_GetTicks() - frameStart) / 1000.0f;

    // Update the player's movement
    player->update(deltaTime);

    // Update the camera to follow the player
    updateCamera();

    // Check if the player has moved to a new chunk and if so, generate new chunks and remove old ones
    int newChunkX = getChunkIndex(player->getX());
    int newChunkY = getChunkIndex(player->getY());

    // Calculate visible chunk boundaries based on camera position
    int visibleStartX = std::floor(static_cast<float>(camera.x) / (chunkSize * 32)) - 1;
    int visibleEndX = std::ceil(static_cast<float>(camera.x + camera.w) / (chunkSize * 32)) + 1;
    int visibleStartY = std::floor(static_cast<float>(camera.y) / (chunkSize * 32)) - 1;
    int visibleEndY = std::ceil(static_cast<float>(camera.y + camera.h) / (chunkSize * 32)) + 1;

    // Generate new chunks within the visible range
    for (int y = visibleStartY; y <= visibleEndY; y++) {
        for (int x = visibleStartX; x <= visibleEndX; x++) {
            if (!gameMap.isChunkGenerated(x, y)) {
                gameMap.generateChunk(x, y, seed);
            }
        }
    }

    // Remove chunks that are out of view
    gameMap.removeOutOfViewChunks(visibleStartX, visibleEndX, visibleStartY, visibleEndY);

    // Calculate how long the update took
    Uint32 frameTime = SDL_GetTicks() - frameStart;

    // If we're running faster than our frame delay, then delay the frame to achieve the desired frame rate
    if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    gameMap.render(renderer, camera);
    player->render(renderer, camera);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Clean up the tileset texture
    Tile::freeTilesetTexture();

    Player::destroyTexture();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool Game::running() {
    return isRunning;
}
