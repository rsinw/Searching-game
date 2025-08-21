// Popup.cpp - Universal popup system for all scenes

extern "C" {
    #include "raylib.h"
}

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

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
    
    // Button system (for future use)
    std::vector<std::string> buttons;
    int selectedButton = 0;
    
    // Input blocking - prevent same-frame input processing
    bool justClosed = false;
    
public:
    bool isActive = false;
    
    Popup() {}
    
    void show(const std::string& text, float x, float y, float width, float height) {
        std::cout << "DEBUG: Popup.show() called with text: " << text << std::endl;
        fullText = text;
        displayText = "";
        typingTimer = 0;
        scaleTimer = 0;
        isTypingComplete = false;
        animationComplete = false;
        isActive = true;
        buttons.clear();  // No buttons for now
        
        targetX = x;
        targetY = y;
        targetWidth = width;
        targetHeight = height;
        std::cout << "DEBUG: Popup isActive set to: " << isActive << std::endl;
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
            std::cout << "DEBUG: ENTER pressed - skipping typing animation" << std::endl;
            displayText = fullText;
            isTypingComplete = true;
            inputConsumed = true;
        }
        // Check for Enter to close popup (when typing is complete and no buttons)
        // Only process this if we didn't just complete typing in this frame
        else if (IsKeyPressed(KEY_ENTER) && isTypingComplete && buttons.empty()) {
            std::cout << "DEBUG: ENTER pressed - closing popup" << std::endl;
            close();
            inputConsumed = true;
        }
        
        return inputConsumed;
    }
    
    void draw(Font& font) {
        if (!isActive) {
            std::cout << "DEBUG: Popup.draw() called but isActive = false" << std::endl;
            return;
        }
        std::cout << "DEBUG: Popup.draw() called with isActive = true" << std::endl;
        
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
        
        // Draw black background rectangle with white border
        DrawRectangle((int)currentX, (int)currentY, (int)currentWidth, (int)currentHeight, BLACK);
        DrawRectangleLines((int)currentX, (int)currentY, (int)currentWidth, (int)currentHeight, WHITE);
        
        // Draw text (only if animation is complete)
        if (animationComplete && !displayText.empty()) {
            float textX = currentX + 20;
            float textY = currentY + 20;
            DrawTextEx(font, displayText.c_str(), {textX, textY}, 24, 1.5f, WHITE);
        }
        
        // Draw continue prompt when typing is complete and no buttons
        if (isTypingComplete && buttons.empty()) {
            const char* continuePrompt = ">";
            Vector2 promptSize = MeasureTextEx(font, continuePrompt, 24, 1.5f);
            float promptX = currentX + currentWidth - promptSize.x - 20;
            float promptY = currentY + currentHeight - promptSize.y - 10;
            DrawTextEx(font, continuePrompt, {promptX, promptY}, 24, 1.5f, WHITE);
        }
    }
    
    void close() {
        isActive = false;
        justClosed = true; // Block input for one frame after closing
    }
    
    bool getIsTypingComplete() const {
        return isTypingComplete;
    }
    
    bool getIsActive() const {
        return isActive;
    }
};