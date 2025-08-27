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
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

// Include Popup implementation
#include "Popup.cpp"

// Include BattleSystem with ECS architecture
#include "BattleSystem.h"

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
    
    // Fade transition system
    enum TransitionState {
        TRANSITION_NONE,
        TRANSITION_FADE_OUT,
        TRANSITION_FADE_IN
    };
    
    TransitionState transitionState = TRANSITION_NONE;
    float fadeAlpha = 0.0f;
    float fadeDuration = 0.5f;  // 0.5 seconds for each fade
    float fadeTimer = 0.0f;

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
        if (transitionState == TRANSITION_NONE) {
            nextScene = newScene;
            sceneChangeRequested = true;
            // Start fade out transition
            transitionState = TRANSITION_FADE_OUT;
            fadeTimer = 0.0f;
            fadeAlpha = 0.0f;
        }
    }
    
    void run() {
        // Initialize first scene (TEMPORARY: Battle for debugging)
        // switchToBattle();  // Skip to battle for sprite alignment debugging
        switchToMainMenu();
        
        while (!WindowShouldClose() && running) {
            update();
            draw();
        }
    }
    
    void update() {
        // Update fade transition
        updateFadeTransition();
        
        // Check if popup is active BEFORE updating anything
        bool popupWasActive = popup.getIsActive();
        
        // Update popup first to handle input
        bool popupConsumedInput = popup.update();
        
        // Update current scene (only if popup wasn't active AND isn't consuming input AND not transitioning)
        if (currentScene && !popupWasActive && !popupConsumedInput && transitionState == TRANSITION_NONE) {
            currentScene->update();
        }
    }
    
    void draw() {
        BeginDrawing();
        
        Color black = {0, 0, 0, 255};
        ClearBackground(black);  // Completely black background
        
        if (currentScene) {
            currentScene->draw();
        }
        
        // Draw popup over everything (universal overlay)
        popup.draw(gameFont);
        
        // Draw fade overlay over everything (including popup)
        if (transitionState != TRANSITION_NONE && fadeAlpha > 0.0f) {
            Color fadeColor = {0, 0, 0, (unsigned char)(fadeAlpha * 255)};
            DrawRectangle(0, 0, screenWidth, screenHeight, fadeColor);
        }
        
        EndDrawing();
    }
    
    // Scene switching methods
    void switchToMainMenu();
    void switchToDungeon();
    void switchToBattle();
    void quit() { running = false; }
    
private:
    void updateFadeTransition();
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
        const char* title = "searching...";
        Vector2 titleSize = MeasureTextEx(game->gameFont, title, 60, 3);
        float titleX = (1280 - titleSize.x) / 2.0f;
        Color white; white.r = 255; white.g = 255; white.b = 255; white.a = 255;
        DrawTextEx(game->gameFont, title, {titleX, 150.0f}, 60, 3, white);
        
        // Draw buttons - centered
        Color yellow; yellow.r = 255; yellow.g = 255; yellow.b = 0; yellow.a = 255;
        Color gray; gray.r = 128; gray.g = 128; gray.b = 128; gray.a = 255;
        for (int i = 0; i < 5; i++) {
            Color buttonColor = (i == selectedButton) ? yellow : white;
            Vector2 buttonSize = MeasureTextEx(game->gameFont, buttons[i], 30, 1.5f);
            float buttonX = (1280 - buttonSize.x) / 2.0f;
            DrawTextEx(game->gameFont, buttons[i], {buttonX, 300.0f + i * 60.0f}, 30, 1.5f, buttonColor);
        }
        
        // Draw instructions - centered
        const char* instructions = "Use UP/DOWN arrows and ENTER";
        Vector2 instructionsSize = MeasureTextEx(game->gameFont, instructions, 24, 1.5f);
        float instructionsX = (1280 - instructionsSize.x) / 2.0f;
        DrawTextEx(game->gameFont, instructions, {instructionsX, 650.0f}, 24, 1.5f, gray);
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

// Dungeon exploration scene implementation
class DungeonScene : public Scene {
private:
    // Game state
    int x_pos = 2;  // Start in center of 5x5 grid (0-4)
    int y_pos = 2;
    int floor = 1;
    int proposed_x_pos = 2;
    int proposed_y_pos = 2;
    
    // UI state management
    enum DungeonState {
        STATE_MOVEMENT,      // Where will you go?
        STATE_CONFIRMATION,  // Sense presence prompts
        STATE_TREASURE,      // Treasure found
        STATE_WALL_HIT       // Wall collision
    };
    
    DungeonState currentState = STATE_MOVEMENT;
    bool stateChange = true;  // Trigger state setup on first frame
    
    // Room types
    enum RoomType {
        ROOM_CLEAR,
        ROOM_TREASURE,
        ROOM_ENEMY,
        ROOM_BOSS,
        ROOM_WALL  // Out of bounds
    };
    
    // Grid layout constants
    static const int GRID_SIZE = 5;
    static const int GRID_START_X = 390;  // Center the 5x5 grid in 1280px width
    static const int GRID_START_Y = 200;
    static const int CELL_SIZE = 100;
    static const int CELL_SPACING = 10;
    
    // Room data storage - [x][y] format
    RoomType roomData[GRID_SIZE][GRID_SIZE];
    
public:
    void onEnter() override {
        currentState = STATE_MOVEMENT;
        stateChange = true;
        // Load or generate room layout for current floor
        loadOrGenerateRooms();
        // TODO: Load x_pos, y_pos from temp_data
    }
    
    void generateRoomsForFloor() {
        // Initialize all rooms as enemy rooms
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                roomData[x][y] = ROOM_ENEMY;
            }
        }
        
        // Set center room (2,2) as clear (starting room)
        roomData[2][2] = ROOM_CLEAR;
        
        // Place 1 boss room randomly (not in center)
        std::vector<std::pair<int,int>> availableSpots;
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (!(x == 2 && y == 2)) { // Not center
                    availableSpots.push_back({x, y});
                }
            }
        }
        
        // Seed random number generator with current time
        srand(time(nullptr));
        
        // Place boss room
        int bossIndex = rand() % availableSpots.size();
        int bossX = availableSpots[bossIndex].first;
        int bossY = availableSpots[bossIndex].second;
        roomData[bossX][bossY] = ROOM_BOSS;
        availableSpots.erase(availableSpots.begin() + bossIndex);
        
        // Place 3 treasure rooms
        for (int i = 0; i < 3; i++) {
            int treasureIndex = rand() % availableSpots.size();
            int treasureX = availableSpots[treasureIndex].first;
            int treasureY = availableSpots[treasureIndex].second;
            roomData[treasureX][treasureY] = ROOM_TREASURE;
            availableSpots.erase(availableSpots.begin() + treasureIndex);
        }
        
        // Remaining rooms stay as ROOM_ENEMY
    }
    
    void saveRoomsToFile() {
        std::string filename = "data/temp_data/rooms.txt";
        std::ofstream file(filename);
        if (file.is_open()) {
            // Write floor number first
            file << floor << std::endl;
            // Write room data in format: x,y,roomtype
            for (int x = 0; x < GRID_SIZE; x++) {
                for (int y = 0; y < GRID_SIZE; y++) {
                    file << x << "," << y << "," << (int)roomData[x][y] << std::endl;
                }
            }
            file.close();
        }
    }
    
    bool loadRoomsFromFile() {
        std::string filename = "data/temp_data/rooms.txt";
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        int savedFloor;
        file >> savedFloor;
        
        // If floor doesn't match, need to regenerate
        if (savedFloor != floor) {
            file.close();
            return false;
        }
        
        std::string line;
        std::getline(file, line); // consume newline after floor number
        
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string xStr, yStr, roomTypeStr;
            
            if (std::getline(ss, xStr, ',') && 
                std::getline(ss, yStr, ',') && 
                std::getline(ss, roomTypeStr)) {
                
                int x = std::stoi(xStr);
                int y = std::stoi(yStr);
                int roomType = std::stoi(roomTypeStr);
                
                if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                    roomData[x][y] = (RoomType)roomType;
                }
            }
        }
        file.close();
        return true;
    }
    
    void loadOrGenerateRooms() {
        if (!loadRoomsFromFile()) {
            generateRoomsForFloor();
            saveRoomsToFile();
        }
    }
    
    void markRoomAsCompleted(int x, int y) {
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            roomData[x][y] = ROOM_CLEAR;
            saveRoomsToFile();
        }
    }
    
    void update() override {
        // Handle state setup when state changes
        if (stateChange) {
            setupCurrentState();
            stateChange = false;
        }
        
        // Handle ESC key to return to main menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->switchToMainMenu();
        }
    }
    
    void draw() override {
        // Define colors at the top of the function
        Color white; white.r = 255; white.g = 255; white.b = 255; white.a = 255;
        Color black; black.r = 0; black.g = 0; black.b = 0; black.a = 255;
        
        // Draw floor number at top middle
        std::string floorText = "Floor " + std::to_string(floor);
        Vector2 floorSize = MeasureTextEx(game->gameFont, floorText.c_str(), 36, 2);
        float floorX = (1280 - floorSize.x) / 2.0f;
        DrawTextEx(game->gameFont, floorText.c_str(), {floorX, 30}, 36, 2, white);
        
        // Draw BACK button (top left)
        DrawTextEx(game->gameFont, "BACK", {50, 30}, 30, 2, white);
        
        // Draw INVENTORY button (top right)
        Vector2 invSize = MeasureTextEx(game->gameFont, "INVENTORY", 30, 2);
        DrawTextEx(game->gameFont, "INVENTORY", {1280 - invSize.x - 50, 30}, 30, 2, white);
        
        // Draw 5x5 grid
        drawGrid();
        
        // Draw player position indicator
        drawPlayerIndicator();
    }
    
private:
    void setupCurrentState() {
        switch (currentState) {
            case STATE_MOVEMENT:
                setupMovementState();
                break;
            case STATE_CONFIRMATION:
                setupConfirmationState();
                break;
            case STATE_TREASURE:
                setupTreasureState();
                break;
            case STATE_WALL_HIT:
                setupWallHitState();
                break;
        }
    }
    
    void setupMovementState() {
        RoomType currentRoom = identifyRoom(x_pos, y_pos);
        
        if (currentRoom == ROOM_CLEAR) {
            // Show movement options with directional buttons as per rpdesign.md
            game->popup.showWithButtons("This room is clear. Where will you go?", 
                200, 500, 880, 150,
                {"North", "East", "South", "West"}, 
                [this](int buttonIndex) { handleMovementButton(buttonIndex); });
        }
    }
    
    void setupConfirmationState() {
        RoomType proposedRoom = identifyRoom(proposed_x_pos, proposed_y_pos);
        std::string promptText;
        
        switch (proposedRoom) {
            case ROOM_TREASURE:
                promptText = "You sense a radiant presence beyond this door. Continue?";
                break;
            case ROOM_ENEMY:
                promptText = "You sense a dangerous presence beyond this door. Continue?";
                break;
            case ROOM_BOSS:
                promptText = "You sense an ominous presence beyond this door. Continue?";
                break;
        }
        
        game->popup.showWithButtons(promptText, 200, 500, 880, 150,
            {"Yes", "No"}, 
            [this](int buttonIndex) { handleConfirmationButton(buttonIndex); });
    }
    
    void setupTreasureState() {
        // TODO: Generate item based on floor
        std::string treasureText = "A chest lays in the center of the room. You open it and receive [IRON SWORD].";
        
        // No buttons - just continue (as per rpdesign.md)
        game->popup.show(treasureText, 200, 500, 880, 150);
        
        // Move to treasure room and mark it as completed
        x_pos = proposed_x_pos;
        y_pos = proposed_y_pos;
        markRoomAsCompleted(x_pos, y_pos);
        
        currentState = STATE_MOVEMENT;
        stateChange = true;
    }
    
    void setupWallHitState() {
        // No buttons - just continue (as per rpdesign.md)
        game->popup.show("A wall stops your path.", 200, 500, 880, 150);
        
        // Return to movement selection
        currentState = STATE_MOVEMENT;
        stateChange = true;
    }
    
    void handleMovementButton(int buttonIndex) {
        // Calculate proposed position based on direction
        proposed_x_pos = x_pos;
        proposed_y_pos = y_pos;
        
        switch (buttonIndex) {
            case 0: // North
                proposed_y_pos--;
                break;
            case 1: // East
                proposed_x_pos++;
                break;
            case 2: // South
                proposed_y_pos++;
                break;
            case 3: // West
                proposed_x_pos--;
                break;
        }
        
        // Check if movement is valid
        RoomType proposedRoom = identifyRoom(proposed_x_pos, proposed_y_pos);
        
        if (proposedRoom == ROOM_WALL) {
            currentState = STATE_WALL_HIT;
            stateChange = true;
        } else if (proposedRoom == ROOM_CLEAR) {
            // Direct movement to clear room
            x_pos = proposed_x_pos;
            y_pos = proposed_y_pos;
            currentState = STATE_MOVEMENT;
            stateChange = true;
        } else {
            // Need confirmation for treasure/enemy/boss rooms
            currentState = STATE_CONFIRMATION;
            stateChange = true;
        }
    }
    
    void handleConfirmationButton(int buttonIndex) {
        if (buttonIndex == 0) { // Yes
            RoomType proposedRoom = identifyRoom(proposed_x_pos, proposed_y_pos);
            
            if (proposedRoom == ROOM_TREASURE) {
                currentState = STATE_TREASURE;
                stateChange = true;
            } else if (proposedRoom == ROOM_ENEMY) {
                // Fade to black and transition to battle scene
                x_pos = proposed_x_pos;
                y_pos = proposed_y_pos;
                game->switchToBattle();
            } else if (proposedRoom == ROOM_BOSS) {
                // Fade to black and transition to boss battle scene
                x_pos = proposed_x_pos;
                y_pos = proposed_y_pos;
                game->switchToBattle();  // For now, same as regular battle
            }
        } else { // No
            // Return to movement selection
            currentState = STATE_MOVEMENT;
            stateChange = true;
        }
    }
    
    RoomType identifyRoom(int x, int y) {
        // Check bounds
        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
            return ROOM_WALL;
        }
        
        // Return room type from generated room data
        return roomData[x][y];
    }
    
    void drawGrid() {
        // Define colors locally
        Color black; black.r = 0; black.g = 0; black.b = 0; black.a = 255;
        Color white; white.r = 255; white.g = 255; white.b = 255; white.a = 255;
        
        // Draw 5x5 grid with black squares, white borders, connected by white lines
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                int cellX = GRID_START_X + x * (CELL_SIZE + CELL_SPACING);
                int cellY = GRID_START_Y + y * (CELL_SIZE + CELL_SPACING);
                
                // Draw black square with white border
                DrawRectangle(cellX, cellY, CELL_SIZE, CELL_SIZE, black);
                DrawRectangleLines(cellX, cellY, CELL_SIZE, CELL_SIZE, white);
            }
        }
        
        // Draw connecting lines between cells
        // Horizontal lines
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE - 1; x++) {
                int startX = GRID_START_X + x * (CELL_SIZE + CELL_SPACING) + CELL_SIZE;
                int endX = GRID_START_X + (x + 1) * (CELL_SIZE + CELL_SPACING);
                int lineY = GRID_START_Y + y * (CELL_SIZE + CELL_SPACING) + CELL_SIZE / 2;
                DrawLine(startX, lineY, endX, lineY, white);
            }
        }
        
        // Vertical lines
        for (int y = 0; y < GRID_SIZE - 1; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                int lineX = GRID_START_X + x * (CELL_SIZE + CELL_SPACING) + CELL_SIZE / 2;
                int startY = GRID_START_Y + y * (CELL_SIZE + CELL_SPACING) + CELL_SIZE;
                int endY = GRID_START_Y + (y + 1) * (CELL_SIZE + CELL_SPACING);
                DrawLine(lineX, startY, lineX, endY, white);
            }
        }
    }
    
    void drawPlayerIndicator() {
        // Define color locally
        Color green; green.r = 0; green.g = 255; green.b = 0; green.a = 255;
        
        // Draw small green box to indicate player position
        int playerX = GRID_START_X + x_pos * (CELL_SIZE + CELL_SPACING) + CELL_SIZE / 2 - 10;
        int playerY = GRID_START_Y + y_pos * (CELL_SIZE + CELL_SPACING) + CELL_SIZE / 2 - 10;
        
        // Draw small green box (20x20) as per rpdesign.md
        DrawRectangle(playerX, playerY, 20, 20, green);
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
    std::cout << "DEBUG: Switching to battle scene!" << std::endl;
    // Create a BattleScene that uses the new Encounter class (Soulseer architecture)
    class BattleScene : public Scene {
    public:
        // No longer need encounter pointer with ECS
        
        BattleScene() {}
        
        ~BattleScene() {
            cleanup_battle_system();
        }
        
        void onEnter() override {
            std::cout << "Entering Battle Scene (ECS style)" << std::endl;
            initialize_battle_system();
        }
        
        void update() override {
            update_battle_system();
            
            // MOBA-style controls adapted for ECS
            Vector2 mousePos = GetMousePosition();
            static bool right_clicking = false;
            static float right_click_timer = 0.0f;
            static int selected_entity = -1;
            static const float LONG_CLICK_THRESHOLD = 0.3f;
            
            // Left click - Unit selection (player units only)
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int clicked_entity = get_entity_at_position(mousePos.x, mousePos.y, 0); // side 0 = player
                if (clicked_entity != -1) {
                    selected_entity = clicked_entity;
                    std::cout << "Selected entity: " << clicked_entity << std::endl;
                } else {
                    selected_entity = -1;
                    std::cout << "Deselected entity" << std::endl;
                }
            }
            
            // Right click with long-click support  
            if (selected_entity != -1) {
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    right_clicking = true;
                    right_click_timer = 0.0f;
                }
                
                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && right_clicking) {
                    right_click_timer += GetFrameTime();
                    
                    if (right_click_timer >= LONG_CLICK_THRESHOLD) {
                        // Long click - continuous movement
                        set_entity_target_location(selected_entity, mousePos.x, mousePos.y);
                        // Don't spam console during long click
                    }
                }
                
                if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && right_clicking) {
                    if (right_click_timer < LONG_CLICK_THRESHOLD) {
                        // Short click - attack enemy or move
                        std::cout << "Right clicked at (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                        
                        // Check for enemy at click position
                        int target_enemy = get_entity_at_position(mousePos.x, mousePos.y, 1); // side 1 = enemy
                        if (target_enemy != -1) {
                            set_entity_target_enemy(selected_entity, target_enemy);
                            std::cout << "Targeting enemy entity: " << target_enemy << std::endl;
                        } else {
                            set_entity_target_location(selected_entity, mousePos.x, mousePos.y);
                            std::cout << "Moving to location (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                        }
                    }
                    right_clicking = false;
                    right_click_timer = 0.0f;
                }
            }
            
            /*
            if (encounter) {
                encounter->update();
                
                // MOBA-style controls: Left click = select units, Right click = move/attack
                
                // Left click - Unit selection
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mousePos = GetMousePosition();
                    bool unit_selected = false;
                    
                    // Try to select player units
                    auto player_group = encounter->return_player_group();
                    for (auto* unit : player_group) {
                        Rectangle unitRect = {unit->rect.x, unit->rect.y, unit->rect.width, unit->rect.height};
                        if (CheckCollisionPointRec(mousePos, unitRect)) {
                            encounter->selected_unit = unit;
                            unit_selected = true;
                            std::cout << "Selected " << unit->type_name << std::endl;
                            break;
                        }
                    }
                    
                    if (!unit_selected) {
                        std::cout << "No unit selected at (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                    }
                }
                
                // Right click - Movement and targeting (with long-click support)
                static bool right_clicking = false;
                static float right_click_timer = 0.0f;
                static const float LONG_CLICK_THRESHOLD = 0.3f; // 300ms for long click
                
                if (encounter->selected_unit) {
                    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        right_clicking = true;
                        right_click_timer = 0.0f;
                    }
                    
                    if (right_clicking) {
                        right_click_timer += GetFrameTime();
                        
                        // Long click - continuous movement
                        if (right_click_timer >= LONG_CLICK_THRESHOLD) {
                            Vector2 mousePos = GetMousePosition();
                            encounter->selected_unit->set_target_location(mousePos.x, mousePos.y);
                            encounter->selected_unit->target_unit = nullptr;
                            // Don't spam console during long click
                        }
                    }
                    
                    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
                        if (right_clicking && right_click_timer < LONG_CLICK_THRESHOLD) {
                            // Short click - target enemy or move
                            Vector2 mousePos = GetMousePosition();
                            std::cout << "Right clicked at (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                            
                            bool enemy_targeted = false;
                            auto enemy_group = encounter->return_enemy_group();
                            for (auto* unit : enemy_group) {
                                Rectangle unitRect = {unit->rect.x, unit->rect.y, unit->rect.width, unit->rect.height};
                                if (CheckCollisionPointRec(mousePos, unitRect)) {
                                    encounter->selected_unit->target_unit = unit;
                                    enemy_targeted = true;
                                    std::cout << "Targeted enemy: " << unit->type_name << std::endl;
                                    break;
                                }
                            }
                            
                            if (!enemy_targeted) {
                                encounter->selected_unit->set_target_location(mousePos.x, mousePos.y);
                                encounter->selected_unit->target_unit = nullptr;
                                std::cout << "Moving to location (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                            }
                        }
                        right_clicking = false;
                        right_click_timer = 0.0f;
                    }
                }
                    
                    // No more manual action triggering - using auto-attack system
                    // Enemies will automatically attack when in range
                
                // Check if battle is over
                if (encounter->over) {
                    // Wait for over_timer, then return to dungeon
                    if (encounter->over_timer <= 0) {
                        game->switchToDungeon();
                    }
                }
            }
            */
        }
        
        void draw() override {
            // Clear background with battle color
            Color battleGray; battleGray.r = 64; battleGray.g = 64; battleGray.b = 64; battleGray.a = 255;
            ClearBackground(battleGray);
            
            render_battle_system();
            
            // Draw controls (MOBA-style)
            Color white; white.r = 255; white.g = 255; white.b = 255; white.a = 255;
            DrawText("Left Click: Select Units | Right Click: Move/Attack | Hold Right: Continuous Move", 10, 680, 20, white);
        }
    };
    
    switchToScene(new BattleScene());
}

void Game::updateFadeTransition() {
    if (transitionState == TRANSITION_NONE) {
        return;
    }
    
    // Update fade timer
    fadeTimer += GetFrameTime();
    
    if (transitionState == TRANSITION_FADE_OUT) {
        // Fade to black (alpha goes from 0 to 1)
        fadeAlpha = fadeTimer / fadeDuration;
        
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 1.0f;
            
            // Perform scene switch at peak of fade-out (screen is black)
            if (sceneChangeRequested) {
                if (currentScene) {
                    currentScene->onExit();
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
            
            // Switch to fade in phase
            transitionState = TRANSITION_FADE_IN;
            fadeTimer = 0.0f;
        }
    }
    else if (transitionState == TRANSITION_FADE_IN) {
        // Fade from black (alpha goes from 1 to 0)
        fadeAlpha = 1.0f - (fadeTimer / fadeDuration);
        
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            // Transition complete
            transitionState = TRANSITION_NONE;
        }
    }
}

int main(void)
{
    Game game;
    game.run();
    return 0;
}