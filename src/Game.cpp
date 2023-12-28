#include "Game.h"
#include <iostream>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), chunkSize(32), seed(12345) {
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
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
        }
        isRunning = true;
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
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        player->setMovingUp(true);
                        break;
                    case SDLK_DOWN:
                        player->setMovingDown(true);
                        break;
                    case SDLK_LEFT:
                        player->setMovingLeft(true);
                        break;
                    case SDLK_RIGHT:
                        player->setMovingRight(true);
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        player->setMovingUp(false);
                        break;
                    case SDLK_DOWN:
                        player->setMovingDown(false);
                        break;
                    case SDLK_LEFT:
                        player->setMovingLeft(false);
                        break;
                    case SDLK_RIGHT:
                        player->setMovingRight(false);
                        break;
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
    frameStart = SDL_GetTicks();

    // Function to calculate the chunk index for a given coordinate
    auto getChunkIndex = [this](int coordinate) -> int {
        if (coordinate >= 0) {
            return coordinate / (32 * chunkSize);
        } else {
            return (coordinate - 32 * chunkSize + 1) / (32 * chunkSize);
        }
    };

    int currentChunkX = getChunkIndex(player->getX());
    int currentChunkY = getChunkIndex(player->getY());

    player->update();
    updateCamera();

    int newChunkX = getChunkIndex(player->getX());
    int newChunkY = getChunkIndex(player->getY());

    // Calculate visible chunk boundaries based on camera position
    int visibleStartX = std::floor(static_cast<float>(camera.x) / (chunkSize * 32)) - 1;
    int visibleEndX = std::ceil(static_cast<float>(camera.x + camera.w) / (chunkSize * 32)) + 1;
    int visibleStartY = std::floor(static_cast<float>(camera.y) / (chunkSize * 32)) - 1;
    int visibleEndY = std::ceil(static_cast<float>(camera.y + camera.h) / (chunkSize * 32)) + 1;

    // Generate new chunks within the visible range
    for (int y = visibleStartY; y <= visibleEndY; ++y) {
        for (int x = visibleStartX; x <= visibleEndX; ++x) {
            if (!gameMap.isChunkGenerated(x, y)) {
                gameMap.generateChunk(x, y, seed);
                std::cout << "Generating Chunk: " << x << ", " << y << std::endl;
            }
        }
    }

    // Remove chunks that are out of view
    auto it = gameMap.chunks.begin();
    while (it != gameMap.chunks.end()) {
        int chunkX = it->first.first;
        int chunkY = it->first.second;
        if (chunkX < visibleStartX || chunkX > visibleEndX || chunkY < visibleStartY || chunkY > visibleEndY) {
            std::cout << "Unrendering Chunk: " << chunkX << ", " << chunkY << std::endl;
            it = gameMap.chunks.erase(it);
        } else {
            ++it;
        }
    }

    frameTime = SDL_GetTicks() - frameStart;
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
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool Game::running() {
    return isRunning;
}
