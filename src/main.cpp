// main.cpp - Entry point for Searching dungeon crawler game

extern "C" {
    #include "raylib.h"
}

// Include the Game class (normally would use headers)
// For simplicity, we'll include the implementation directly

#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

// Include Popup implementation
#include "Popup.cpp"

// Forward declarations
class Scene;
class Popup;

class Scene {
public:
    class Game* game = nullptr;
    
    Scene() {}
    virtual ~Scene() {}
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void update() = 0;
    virtual void draw() = 0;
};

class Game {
private:
    int screenWidth = 1280;
    int screenHeight = 720;
    bool running = true;
    
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
    bool sceneChangeRequested = false;

public:
    Font gameFont;  // Global font for all text
    Popup popup;    // Universal popup system
    
    Game() {
        InitWindow(screenWidth, screenHeight, "Searching");
        SetExitKey(KEY_NULL);  // Disable ESC auto-closing window
        SetTargetFPS(60);
        
        // Load custom font
        gameFont = LoadFont("assets/fonts/Ithaca-LVB75.ttf");
        if (gameFont.texture.id == 0) {
            // Fallback to default font if loading fails
            gameFont = GetFontDefault();
        }
    }
    
    ~Game() {
        if (currentScene) delete currentScene;
        if (gameFont.texture.id != GetFontDefault().texture.id) {
            UnloadFont(gameFont);  // Only unload if it's not the default font
        }
        CloseWindow();
    }
    
    void switchToScene(Scene* newScene) {
        nextScene = newScene;
        sceneChangeRequested = true;
    }
    
    void run() {
        // Initialize first scene (MainMenu)
        switchToMainMenu();
        
        while (!WindowShouldClose() && running) {
            update();
            draw();
        }
    }
    
    void update() {
        // Handle scene switching
        if (sceneChangeRequested) {
            if (currentScene) {
                delete currentScene;
            }
            currentScene = nextScene;
            nextScene = nullptr;
            sceneChangeRequested = false;
            
            if (currentScene) {
                currentScene->game = this;
                currentScene->onEnter();
            }
        }
        
        // Check if popup is active BEFORE updating anything
        bool popupWasActive = popup.getIsActive();
        
        // Update popup first to handle input
        bool popupConsumedInput = popup.update();
        
        // Update current scene (only if popup wasn't active AND isn't consuming input)
        if (currentScene && !popupWasActive && !popupConsumedInput) {
            currentScene->update();
        }
    }
    
    void draw() {
        BeginDrawing();
        
        ClearBackground(BLACK);  // Completely black background
        
        if (currentScene) {
            currentScene->draw();
        }
        
        // Draw popup over everything (universal overlay)
        popup.draw(gameFont);
        
        EndDrawing();
    }
    
    // Scene switching methods
    void switchToMainMenu();
    void switchToDungeon();
    void switchToBattle();
    void quit() { running = false; }
};

// MainMenuScene implementation
class MainMenuScene : public Scene {
private:
    int selectedButton = 0;
    const char* buttons[5] = {"PLAY", "LOAD", "SAVE", "EXIT", "TEST POPUP"};
    
public:
    void onEnter() override {
        selectedButton = 0;
    }
    
    void update() override {
        // Handle input
        if (IsKeyPressed(KEY_UP)) {
            selectedButton = (selectedButton - 1 + 5) % 5;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selectedButton = (selectedButton + 1) % 5;
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            handleButtonPress();
        }
    }
    
    void draw() override {
        // Draw title - centered
        const char* title = "SEARCHING";
        Vector2 titleSize = MeasureTextEx(game->gameFont, title, 60, 3);
        float titleX = (1280 - titleSize.x) / 2.0f;
        DrawTextEx(game->gameFont, title, {titleX, 150.0f}, 60, 3, WHITE);
        
        // Draw buttons - centered
        for (int i = 0; i < 5; i++) {
            Color buttonColor = (i == selectedButton) ? YELLOW : WHITE;
            Vector2 buttonSize = MeasureTextEx(game->gameFont, buttons[i], 30, 1.5f);
            float buttonX = (1280 - buttonSize.x) / 2.0f;
            DrawTextEx(game->gameFont, buttons[i], {buttonX, 300.0f + i * 60.0f}, 30, 1.5f, buttonColor);
        }
        
        // Draw instructions - centered
        const char* instructions = "Use UP/DOWN arrows and ENTER";
        Vector2 instructionsSize = MeasureTextEx(game->gameFont, instructions, 24, 1.5f);
        float instructionsX = (1280 - instructionsSize.x) / 2.0f;
        DrawTextEx(game->gameFont, instructions, {instructionsX, 650.0f}, 24, 1.5f, GRAY);
    }
    
private:
    void handleButtonPress() {
        switch (selectedButton) {
            case 0: // PLAY
                game->switchToDungeon();
                break;
            case 1: // LOAD
                // TODO: Implement load menu
                break;
            case 2: // SAVE
                // TODO: Implement save functionality
                break;
            case 3: // EXIT
                game->quit();
                break;
            case 4: // TEST POPUP
                // Show popup at bottom of screen
                std::cout << "DEBUG: TEST POPUP button pressed!" << std::endl;
                game->popup.show("Here is an implementation of a popup!", 200, 500, 880, 150);
                std::cout << "DEBUG: Popup.show() called" << std::endl;
                break;
        }
    }
};

// Simple test scene for dungeon
class DungeonScene : public Scene {
public:
    void update() override {
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->switchToMainMenu();
        }
    }
    
    void draw() override {
        // Draw title - centered
        const char* title = "DUNGEON SCENE";
        Vector2 titleSize = MeasureTextEx(game->gameFont, title, 45, 3);
        float titleX = (1280 - titleSize.x) / 2.0f;
        DrawTextEx(game->gameFont, title, {titleX, 300.0f}, 45, 3, WHITE);
        
        // Draw instructions - centered
        const char* instructions = "Press ESC to return to main menu";
        Vector2 instructionsSize = MeasureTextEx(game->gameFont, instructions, 24, 1.5f);
        float instructionsX = (1280 - instructionsSize.x) / 2.0f;
        DrawTextEx(game->gameFont, instructions, {instructionsX, 400.0f}, 24, 1.5f, GRAY);
    }
};

// Scene switching implementations
void Game::switchToMainMenu() {
    switchToScene(new MainMenuScene());
}

void Game::switchToDungeon() {
    switchToScene(new DungeonScene());
}

void Game::switchToBattle() {
    // TODO: Implement battle scene
}

int main(void)
{
    Game game;
    game.run();
    return 0;
}