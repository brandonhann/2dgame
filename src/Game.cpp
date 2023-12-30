#include "Game.h"
#include "Player.h"
#include "TitleScreen.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_ttf.h>

Game::Game() 
    : gameState(GameState::TITLE_SCREEN), 
      titleScreen(nullptr), // titleScreen should be initialized here
      isRunning(false), 
      window(nullptr), 
      renderer(nullptr), 
      camera({0, 0, 800, 600}), // Initialize camera
      player(new Player(100, 100)), // Initialize player
      FPS(60), 
      frameDelay(1000 / FPS), 
      frameStart(0), 
      frameTime(0),
      chunkSize(32), 
      seed(12345), 
      gameMap(seed),
      seedNeedsUpdate(false),
      displaySeedMessage(true),
      seedMessageStartTime(SDL_GetTicks())
{ 
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
                Player::loadPlayerTexture(renderer, "assets/player_sprite_map.png");
            }

            // Set draw color for renderer to white
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
            isRunning = false;  // This will handle the window close button in any state.
            return; // Exit the function early
        }

        if (gameState == GameState::TITLE_SCREEN) {
            titleScreen->handleEvents(event, gameState);
            if (gameState == GameState::GAMEPLAY) {
                seedNeedsUpdate = true; // Flag to update the seed in the update method
            }
        } else if (gameState == GameState::GAMEPLAY) {
            // Handle events specific to the gameplay
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                player->handleInput(event); // Delegate to player input handling
            }
        }

        // Handling window resize event
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int newWidth = event.window.data1;
            int newHeight = event.window.data2;

            // Update camera size with the new window dimensions
            camera.w = newWidth;
            camera.h = newHeight;

            // Update title screen layout if we're on the title screen
            if (gameState == GameState::TITLE_SCREEN) {
                titleScreen->handleWindowSizeChange(newWidth, newHeight);
            }
        }
    }
}

void Game::updateCamera() {
    // Center the camera over the player
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;
}

void Game::update() {
    // Check if the game state is GAMEPLAY and if the seed needs to be updated
    if (gameState == GameState::GAMEPLAY && seedNeedsUpdate) {
        seed = static_cast<unsigned int>(time(nullptr)); // Generate a new seed
        gameMap = Map(seed); // Reinitialize the map with the new seed
        seedNeedsUpdate = false;
    }

    // Rest of the update logic remains the same
    Uint32 frameStart = SDL_GetTicks();

    auto getChunkIndex = [this](int coordinate) -> int {
        return coordinate / (32 * chunkSize);
    };

    float deltaTime = (SDL_GetTicks() - frameStart) / 1000.0f;

    player->update(deltaTime);
    updateCamera();

    int newChunkX = getChunkIndex(player->getX());
    int newChunkY = getChunkIndex(player->getY());

    int visibleStartX = std::floor(static_cast<float>(camera.x) / (chunkSize * 32)) - 1;
    int visibleEndX = std::ceil(static_cast<float>(camera.x + camera.w) / (chunkSize * 32)) + 1;
    int visibleStartY = std::floor(static_cast<float>(camera.y) / (chunkSize * 32)) - 1;
    int visibleEndY = std::ceil(static_cast<float>(camera.y + camera.h) / (chunkSize * 32)) + 1;

    for (int y = visibleStartY; y <= visibleEndY; y++) {
        for (int x = visibleStartX; x <= visibleEndX; x++) {
            if (!gameMap.isChunkGenerated(x, y)) {
                gameMap.generateChunk(x, y, seed);
            }
        }
    }

    gameMap.removeOutOfViewChunks(visibleStartX, visibleEndX, visibleStartY, visibleEndY);

    Uint32 frameTime = SDL_GetTicks() - frameStart;
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
            // Render game-related objects here
            gameMap.render(renderer, camera);
            player->render(renderer, camera);

            // Display the seed message only once when entering the gameplay state
            if (displaySeedMessage) {
                std::string seedMessage = "Generated with seed: " + std::to_string(seed);
                // Display the message on the screen
                // Note: Add rendering logic for displaying the seed message on the screen
                std::cout << seedMessage << std::endl; // Temporary console output for testing
                
                // After displaying the message, set the flag to false to prevent repeated displaying
                displaySeedMessage = false;
            }

            // More gameplay rendering logic can be added here

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
