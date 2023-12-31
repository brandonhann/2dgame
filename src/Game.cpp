#include "Game.h"
#include "Player.h"
#include "TitleScreen.h"
#include "Camera.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>

Game::Game() 
    : gameState(GameState::TITLE_SCREEN), 
      titleScreen(nullptr),
      isRunning(false), 
      window(nullptr), 
      renderer(nullptr), 
      camera(new Camera(0, 0, 800, 600)), // Corrected initialization
      player(new Player(100, 100)), // Initialize player
      FPS(60), 
      frameDelay(1000 / FPS), 
      frameStart(0), 
      frameTime(0),
      chunkSize(32), 
      seed(12345), 
      gameMap(seed), // Initialize map with seed
      seedNeedsUpdate(false),
      displaySeedMessage(true),
      seedMessageStartTime(SDL_GetTicks()),
      lastFrameStart(SDL_GetTicks())
{
    // Initialize player and camera only once, remove re-initialization from here
    int initialChunkX = player->getX() / (32 * chunkSize);
    int initialChunkY = player->getY() / (32 * chunkSize);

    for (int y = initialChunkY - 1; y <= initialChunkY + 1; y++) {
        for (int x = initialChunkX - 1; x <= initialChunkX + 1; x++) {
            gameMap.generateChunk(x, y, seed);
        }
    }
}

Game::~Game() {
    delete camera;
    delete player;
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
                Tile::loadTilesetTexture(renderer, ROOT_PATH "assets/tilemap.png");
                // Load the player texture
                Player::loadPlayerTexture(renderer, ROOT_PATH "assets/player_sprite_map.png");
            }

            // Set draw color for renderer to white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
            std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        }
        isRunning = true;
    } else {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    // Start accepting text input
    SDL_StartTextInput();

    titleScreen = new TitleScreen(renderer, width, height);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            return;
        }

        if (gameState == GameState::TITLE_SCREEN) {
            titleScreen->handleEvents(event, gameState);
            if (gameState == GameState::GAMEPLAY) {
                std::string userInputSeed = titleScreen->getUIManagerSeedText();
                seed = hashStringToUnsignedInt(userInputSeed);
                gameMap = Map(seed);
                seedNeedsUpdate = false;
            }
        } else if (gameState == GameState::GAMEPLAY) {
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                player->handleInput(event);
            }
        }

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int newWidth = event.window.data1;
            int newHeight = event.window.data2;

            SDL_Rect& cameraRect = camera->getCameraRect();
            cameraRect.w = newWidth;
            cameraRect.h = newHeight;

            if (gameState == GameState::TITLE_SCREEN) {
                titleScreen->handleWindowSizeChange(newWidth, newHeight);
            }
        }
    }
}

unsigned int Game::hashStringToUnsignedInt(const std::string& textSeed) {
    unsigned int hash = 0;
    for (char c : textSeed) {
        hash = hash * 31 + c;
    }
    return hash;
}

void Game::update() {
    if (gameState == GameState::GAMEPLAY && seedNeedsUpdate) {
        seed = static_cast<unsigned int>(time(nullptr));
        gameMap = Map(seed);
        seedNeedsUpdate = false;
    }

    // Capture the start time of the current frame
    Uint32 frameStart = SDL_GetTicks();

    // Calculate deltaTime using the difference between the current frame start time and the last frame start time
    float deltaTime = (frameStart - lastFrameStart) / 1000.0f;
    lastFrameStart = frameStart;  // Update lastFrameStart for the next frame

    player->update(deltaTime);
    camera->update(player->getX(), player->getY());

    SDL_Rect cameraRect = camera->getCameraRect();
    int visibleStartX = std::floor(static_cast<float>(cameraRect.x) / (chunkSize * 32)) - 1;
    int visibleEndX = std::ceil(static_cast<float>(cameraRect.x + cameraRect.w) / (chunkSize * 32)) + 1;
    int visibleStartY = std::floor(static_cast<float>(cameraRect.y) / (chunkSize * 32)) - 1;
    int visibleEndY = std::ceil(static_cast<float>(cameraRect.y + cameraRect.h) / (chunkSize * 32)) + 1;

    for (int y = visibleStartY; y <= visibleEndY; y++) {
        for (int x = visibleStartX; x <= visibleEndX; x++) {
            if (!gameMap.isChunkGenerated(x, y)) {
                gameMap.generateChunk(x, y, seed);
            }
        }
    }

    gameMap.removeOutOfViewChunks(visibleStartX, visibleEndX, visibleStartY, visibleEndY);

    // Calculate how long the current frame took to process
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    // If the frame processed faster than our target frame rate, delay the next frame
    if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    switch (gameState) {
        case GameState::TITLE_SCREEN:
            titleScreen->render();
            break;

        case GameState::GAMEPLAY:
            if (displaySeedMessage) {
                // Adjusted to use the new method in TitleScreen that accesses UIManager's getSeedText()
                std::string seedMessage = "Generated with seed: " + titleScreen->getUIManagerSeedText();
                std::cout << seedMessage << std::endl;
                displaySeedMessage = false;
            }

            SDL_Rect cameraRect = camera->getCameraRect();
            gameMap.render(renderer, cameraRect);
            player->render(renderer, cameraRect);

            SDL_RenderPresent(renderer);
            break;
    }
}

void Game::clean() {
    delete titleScreen;
    Tile::freeTilesetTexture();
    Player::destroyTexture();
    SDL_StopTextInput();
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool Game::running() {
    return isRunning;
}

void Game::setSeed(unsigned int newSeed) {
    seed = newSeed;
    gameMap = Map(seed); // Reinitialize the map with the new seed
}
