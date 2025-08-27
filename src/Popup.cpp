// Popup.cpp - Universal popup system for all scenes

extern "C" {
    #include "raylib.h"
}

#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <functional>

class Popup {
private:
    std::string fullText;
    std::string displayText;
    int typingTimer = 0;
    int typingSpeed = 3;  // Frames per character
    bool isTypingComplete = false;
    
    // Animation properties
    float scaleTimer = 0;
    float maxScaleTime = 15;  // Frames to reach full size
    bool animationComplete = false;
    
    // Position and size
    float targetX, targetY, targetWidth, targetHeight;
    
    // Button system
    std::vector<std::string> buttons;
    int selectedButton = 0;
    std::function<void(int)> buttonCallback = nullptr;
    
    // Input blocking - prevent same-frame input processing
    bool justClosed = false;
    
public:
    bool isActive = false;
    
    Popup() {}
    
    void show(const std::string& text, float x, float y, float width, float height) {
        fullText = text;
        displayText = "";
        typingTimer = 0;
        scaleTimer = 0;
        isTypingComplete = false;
        animationComplete = false;
        isActive = true;
        buttons.clear();
        buttonCallback = nullptr;
        selectedButton = 0;
        
        targetX = x;
        targetY = y;
        targetWidth = width;
        targetHeight = height;
    }
    
    void showWithButtons(const std::string& text, float x, float y, float width, float height, 
                        const std::vector<std::string>& buttonLabels, std::function<void(int)> callback) {
        show(text, x, y, width, height);
        buttons = buttonLabels;
        buttonCallback = callback;
        selectedButton = 0;
    }
    
    bool update() {
        // Handle the frame after closing to block input
        if (justClosed) {
            justClosed = false;
            return true; // Still consume input for one frame after closing
        }
        
        if (!isActive) return false;
        
        bool inputConsumed = false;
        
        // Handle scaling animation
        if (!animationComplete) {
            scaleTimer++;
            if (scaleTimer >= maxScaleTime) {
                animationComplete = true;
            }
        }
        
        // Handle typing animation (only after scaling is done)
        if (animationComplete && !isTypingComplete) {
            typingTimer++;
            if (typingTimer >= typingSpeed) {
                typingTimer = 0;
                if (displayText.length() < fullText.length()) {
                    displayText += fullText[displayText.length()];
                } else {
                    isTypingComplete = true;
                }
            }
        }
        
        // Check for Enter to skip typing
        if (IsKeyPressed(KEY_ENTER) && !isTypingComplete) {
            displayText = fullText;
            isTypingComplete = true;
            inputConsumed = true;
        }
        // Handle button navigation and selection when typing is complete
        else if (isTypingComplete) {
            if (!buttons.empty()) {
                // Handle button navigation
                if (IsKeyPressed(KEY_UP)) {
                    selectedButton = (selectedButton - 1 + buttons.size()) % buttons.size();
                    inputConsumed = true;
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    selectedButton = (selectedButton + 1) % buttons.size();
                    inputConsumed = true;
                }
                if (IsKeyPressed(KEY_LEFT) && buttons.size() > 2) {
                    // For 2x2 layout (North/East, South/West)
                    if (selectedButton == 1) selectedButton = 0; // East -> North
                    else if (selectedButton == 3) selectedButton = 2; // West -> South
                    inputConsumed = true;
                }
                if (IsKeyPressed(KEY_RIGHT) && buttons.size() > 2) {
                    // For 2x2 layout (North/East, South/West)
                    if (selectedButton == 0) selectedButton = 1; // North -> East
                    else if (selectedButton == 2) selectedButton = 3; // South -> West
                    inputConsumed = true;
                }
                
                // Handle button selection
                if (IsKeyPressed(KEY_ENTER)) {
                    if (buttonCallback) {
                        buttonCallback(selectedButton);
                    }
                    close();
                    inputConsumed = true;
                }
            } else {
                // No buttons - just continue with Enter
                if (IsKeyPressed(KEY_ENTER)) {
                    close();
                    inputConsumed = true;
                }
            }
        }
        
        return inputConsumed;
    }
    
    void draw(Font& font) {
        if (!isActive) return;
        
        // Calculate current scale based on animation
        float scale = 1.0f;
        if (!animationComplete) {
            scale = scaleTimer / maxScaleTime;
            scale = std::min(scale, 1.0f);
        }
        
        // Calculate current dimensions
        float currentWidth = targetWidth * scale;
        float currentHeight = targetHeight * scale;
        float currentX = targetX + (targetWidth - currentWidth) / 2.0f;
        float currentY = targetY + (targetHeight - currentHeight) / 2.0f;
        
        // Draw main text box - black background with white border
        DrawRectangle((int)currentX, (int)currentY, (int)currentWidth, (int)currentHeight, BLACK);
        DrawRectangleLines((int)currentX, (int)currentY, (int)currentWidth, (int)currentHeight, WHITE);
        
        // Draw text (only if animation is complete)
        if (animationComplete && !displayText.empty()) {
            float textX = currentX + 20;
            float textY = currentY + 20;
            DrawTextEx(font, displayText.c_str(), {textX, textY}, 24, 1.5f, WHITE);
        }
        
        // Draw buttons or continue prompt when typing is complete
        if (isTypingComplete) {
            if (!buttons.empty()) {
                drawButtons(font, currentX, currentY, currentWidth, currentHeight);
            } else {
                // Draw continue prompt (>) in bottom right
                const char* continuePrompt = ">";
                Vector2 promptSize = MeasureTextEx(font, continuePrompt, 24, 1.5f);
                float promptX = currentX + currentWidth - promptSize.x - 20;
                float promptY = currentY + currentHeight - promptSize.y - 10;
                DrawTextEx(font, continuePrompt, {promptX, promptY}, 24, 1.5f, WHITE);
            }
        }
    }
    
    bool getIsTypingComplete() const {
        return isTypingComplete;
    }
    
    bool getIsActive() const {
        return isActive;
    }
    
private:
    void drawButtons(Font& font, float textBoxX, float textBoxY, float textBoxWidth, float textBoxHeight) {
        if (buttons.size() == 2) {
            // Yes/No style - side by side in choice box next to text box
            drawChoiceBox(font, textBoxX, textBoxY, textBoxWidth, textBoxHeight);
        } else if (buttons.size() == 4) {
            // Movement style - 2x2 grid in text box
            drawMovementButtons(font, textBoxX, textBoxY, textBoxWidth, textBoxHeight);
        }
    }
    
    void drawChoiceBox(Font& font, float textBoxX, float textBoxY, float textBoxWidth, float textBoxHeight) {
        // Choice box directly on the right side of text box, bottom-aligned
        float choiceBoxWidth = 150;
        float choiceBoxHeight = buttons.size() * 35 + 10; // Dynamic height based on button count
        float choiceBoxX = textBoxX + textBoxWidth + 10; // 10px gap from text box
        float choiceBoxY = textBoxY + textBoxHeight - choiceBoxHeight; // Bottom-aligned with text box
        
        // Draw choice box - black background with white border
        DrawRectangle((int)choiceBoxX, (int)choiceBoxY, (int)choiceBoxWidth, (int)choiceBoxHeight, BLACK);
        DrawRectangleLines((int)choiceBoxX, (int)choiceBoxY, (int)choiceBoxWidth, (int)choiceBoxHeight, WHITE);
        
        // Draw choices stacked vertically, first choice at bottom level of text box
        for (int i = 0; i < buttons.size(); i++) {
            Color textColor = (i == selectedButton) ? YELLOW : WHITE;
            // Start from bottom and stack upward (reverse order visually)
            float buttonY = choiceBoxY + choiceBoxHeight - 35 - (i * 35);
            DrawTextEx(font, buttons[i].c_str(), {choiceBoxX + 15, buttonY}, 20, 1.5f, textColor);
        }
    }
    
    void drawMovementButtons(Font& font, float textBoxX, float textBoxY, float textBoxWidth, float textBoxHeight) {
        // 2x2 grid layout within the text box: North East / South West
        float buttonSpacing = 100;
        float startX = textBoxX + textBoxWidth - 220;
        float startY = textBoxY + textBoxHeight - 80;
        
        // Button positions: [0]=North, [1]=East, [2]=South, [3]=West
        Vector2 positions[4] = {
            {startX, startY},           // North (top-left)
            {startX + buttonSpacing, startY},     // East (top-right)
            {startX, startY + 30},      // South (bottom-left)
            {startX + buttonSpacing, startY + 30} // West (bottom-right)
        };
        
        for (int i = 0; i < 4; i++) {
            Color textColor = (i == selectedButton) ? YELLOW : WHITE;
            DrawTextEx(font, buttons[i].c_str(), positions[i], 20, 1.5f, textColor);
        }
    }
    
    void close() {
        isActive = false;
        justClosed = true; // Block input for one frame after closing
    }
};