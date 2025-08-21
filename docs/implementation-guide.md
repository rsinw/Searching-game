# Implementation Guide

This document provides specific implementation details, pseudocode, and code examples for implementing the game systems outlined in game-vision.md.

## Effects System Implementation

### Base Effect Class
```cpp
// In Effects.cpp
class Effect {
public:
    bool remove = false;           // Marked for removal flag
    int duration = -1;             // -1 = permanent, >0 = temporary
    
    virtual void onAdd(Unit* unit) = 0;      // Apply initial effect
    virtual void update(Unit* unit) = 0;     // Called each frame
    virtual void onRemove(Unit* unit) = 0;   // Cleanup when removed
    virtual ~Effect() = default;             // Virtual destructor
};
```

### Unit Class Effect Integration
```cpp
// In Unit.cpp - add to Unit class
class Unit {
private:
    std::vector<std::unique_ptr<Effect>> effects;
    
    // Immunity flags
    bool poisonImmune = false;
    bool stunImmune = false;
    bool fireImmune = false;
    bool freezeImmune = false;
    
public:
    // Add effect to unit
    void addEffect(std::unique_ptr<Effect> effect) {
        effect->onAdd(this);
        effects.push_back(std::move(effect));
    }
    
    // Update all effects (call each frame)
    void updateEffects() {
        // Update all effects
        for (auto& effect : effects) {
            effect->update(this);
        }
        
        // Remove marked effects
        std::erase_if(effects, [this](const std::unique_ptr<Effect>& effect) {
            if (effect->remove) {
                effect->onRemove(this);
                return true;
            }
            return false;
        });
    }
    
    // Immunity check functions
    bool isPoisonImmune() const { return poisonImmune; }
    bool isStunImmune() const { return stunImmune; }
    // Add getters for other immunities
};
```

### Example Effect Implementations
```cpp
// Health Bonus Effect (temporary)
class HealthBonusEffect : public Effect {
    int healthBonus;
public:
    HealthBonusEffect(int bonus, int dur) : healthBonus(bonus) { duration = dur; }
    
    void onAdd(Unit* unit) override {
        unit->max_hp += healthBonus;
        unit->hp += healthBonus;
    }
    
    void update(Unit* unit) override {
        if (duration > 0) {
            duration--;
            if (duration <= 0) {
                remove = true;
            }
        }
    }
    
    void onRemove(Unit* unit) override {
        unit->max_hp -= healthBonus;
        if (unit->hp > unit->max_hp) unit->hp = unit->max_hp;
    }
};

// Poison Immunity Effect (permanent from equipment)
class PoisonImmunityEffect : public Effect {
public:
    PoisonImmunityEffect() { duration = -1; } // Permanent
    
    void onAdd(Unit* unit) override {
        unit->poisonImmune = true;
    }
    
    void update(Unit* unit) override {
        // Permanent effect, no countdown needed
    }
    
    void onRemove(Unit* unit) override {
        unit->poisonImmune = false;
    }
};

// Attack Bonus Effect (permanent from equipment)
class AttackBonusEffect : public Effect {
    int attackBonus;
public:
    AttackBonusEffect(int bonus) : attackBonus(bonus) { duration = -1; }
    
    void onAdd(Unit* unit) override {
        unit->atk += attackBonus;
    }
    
    void update(Unit* unit) override {
        // Permanent effect, no countdown needed
    }
    
    void onRemove(Unit* unit) override {
        unit->atk -= attackBonus;
    }
};
```

### Effect Application with Immunity Checks
```cpp
// Helper functions for applying effects
void applyPoisonEffect(Unit* target, int damage, int duration) {
    if (!target->isPoisonImmune()) {
        target->addEffect(std::make_unique<PoisonEffect>(damage, duration));
    }
    // If immune, effect is blocked
}

void applyStunEffect(Unit* target, int duration) {
    if (!target->isStunImmune()) {
        target->addEffect(std::make_unique<StunEffect>(duration));
    }
}
```

## Animation System Implementation

### Animation Class Structure
```cpp
// In Animation.cpp
class Animation {
private:
    std::vector<Texture2D> frames;
    int currentFrame = 0;
    int frameTimer = 0;
    int frameDuration = 10;  // Default timing
    bool repeat = true;
    
public:
    Animation(const std::string& filepath, int frameCount, int frameSize) {
        Texture2D spritesheet = LoadTexture(filepath.c_str());
        
        for(int i = 0; i < frameCount; i++) {
            Rectangle sourceRect = {
                (float)(i * frameSize),  // x position
                0,                       // y position  
                (float)frameSize,        // width
                (float)frameSize         // height
            };
            
            Image frameImage = ImageFromTexture(spritesheet, sourceRect);
            frames.push_back(LoadTextureFromImage(frameImage));
            UnloadImage(frameImage);
        }
        
        UnloadTexture(spritesheet);
    }
    
    void update() {
        frameTimer++;
        if (frameTimer >= frameDuration) {
            frameTimer = 0;
            currentFrame++;
            if (currentFrame >= frames.size()) {
                if (repeat) {
                    currentFrame = 0;
                } else {
                    currentFrame = frames.size() - 1;
                }
            }
        }
    }
    
    Texture2D getCurrentFrame() const {
        return frames[currentFrame];
    }
    
    bool isComplete() const {
        return !repeat && currentFrame == frames.size() - 1;
    }
    
    void reset() {
        currentFrame = 0;
        frameTimer = 0;
    }
    
    // Speed scaling for Action integration
    void setDuration(int totalFrames) {
        frameDuration = totalFrames / frames.size();
        if (frameDuration <= 0) frameDuration = 1;
    }
};
```

## Save/Load System Implementation

### File I/O Functions (Utils.cpp)
```cpp
// Key-value file reading
std::map<std::string, std::string> readKeyValueFile(const std::string& filepath) {
    std::map<std::string, std::string> data;
    std::ifstream file(filepath);
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // Remove quotes from value
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            data[key] = value;
        }
    }
    
    return data;
}

// Write key-value file
void writeKeyValueFile(const std::string& filepath, const std::map<std::string, std::string>& data) {
    std::ofstream file(filepath);
    for (const auto& pair : data) {
        file << pair.first << "=\"" << pair.second << "\"\n";
    }
}

// Copy directory structure (for save/load)
void copyDirectory(const std::string& source, const std::string& destination) {
    // Implementation depends on filesystem library
    // Copy all files and subdirectories from source to destination
}
```

### Equipment Loading Implementation
```cpp
// Load equipment effects on battle scene startup
void loadEquipmentEffects(Unit* unit, const std::string& saveDirectory) {
    std::string itemsPath = saveDirectory + "/char_1/items.txt";
    auto equippedItems = readKeyValueFile(itemsPath);
    
    // Check each equipment slot
    if (equippedItems["WEAPON"] != "NONE") {
        applyEquipmentEffects(unit, equippedItems["WEAPON"]);
    }
    if (equippedItems["HEAD"] != "NONE") {
        applyEquipmentEffects(unit, equippedItems["HEAD"]);
    }
    // Continue for all equipment slots...
}

// Apply effects from specific equipment item
void applyEquipmentEffects(Unit* unit, const std::string& itemName) {
    // Look up item in item database (to be implemented)
    // Apply stat bonuses and effects based on item data
    
    // Example for "IRON_HELMET":
    if (itemName == "IRON_HELMET") {
        unit->addEffect(std::make_unique<AttackBonusEffect>(2));
        unit->addEffect(std::make_unique<PoisonImmunityEffect>());
    }
}
```

## Notes

- This implementation guide should be updated as systems are refined
- Code examples use modern C++ features (C++14/17/20)
- Memory management uses smart pointers for safety
- All file I/O should include proper error handling in final implementation