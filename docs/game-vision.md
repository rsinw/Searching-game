# Game Vision and Development Plan

This document captures the current vision for the "Searching" dungeon crawler game and the development roadmap based on discussions and existing design documents.

## Architecture Overview

### Scene-Based System
The game is built around a **scene-based architecture** where different screens represent different game states:

1. **Main Menu Scene**
2. **Dungeon Crawler Select Scene** 
3. **Battle Encounter Scene**

### Scene Transitions
- All scene transitions use a **fade-to-black** effect
- Screen fades out to black, then fades back in to the new scene
- Provides smooth, polished transitions between game states

### Popup System
A universal overlay system that can appear on any scene:

**Visual Design:**
- White bordered rectangle displayed over current scene
- Acts as a modal dialog system

**Text Display:**
- Text appears with a "typing" animation (character by character)
- User can press Enter to skip the typing animation
- After text is fully displayed, shows interaction options

**Interaction Options:**
- **Buttons**: Customizable actions based on context (as described in rpdesign.md)
- **Continue Prompt**: When no buttons are present, shows ">" symbol in bottom right
  - User presses Enter to dismiss popup

## Main Menu Scene

### Buttons
- **PLAY**: Loads current temp_data and transitions to dungeon crawler scene
- **LOAD**: Shows save file selection interface
- **SAVE**: Saves current temp_data to a save file (with confirmation popup)
- **EXIT**: Quits the game (with confirmation popup)

### Button Functionality Details

**PLAY Button:**
- Reads existing temp_data from filesystem
- Transitions directly to dungeon crawler scene with current progress

**LOAD Button:**
- Displays list of available save files
- User can select which save to load
- Uses popup system for confirmation ("Are you sure you want to load this save?")

**SAVE Button:**
- Saves current temp_data to a permanent save file
- Uses popup system for confirmation and potentially save slot naming

**EXIT Button:**
- Uses popup system for confirmation ("Are you sure you want to quit?")
- Cleanly exits the application

## Technical Implementation Notes

### Data Management
Based on existing docs/pseudocode.md:
- **temp_data**: Temporary session data, wiped and recreated on startup
- **Save files**: Permanent storage of game progress
- Data structure includes:
  - Character stats (HP, ATK, DEF, LVL, EXP)
  - Inventory items (HEAD, BODY, WEAPON, TRINKET1, TRINKET2)
  - Game progress (FLOOR, x_pos, y_pos, cleared rooms)

### C++ Implementation Considerations
- Cannot use raylib color macros (like RAYWHITE) - must define Color structs manually
- Use `extern "C"` wrapper for raylib headers
- Follow state machine pattern for game logic
- 1280x720 window resolution, 60 FPS target

## Development Roadmap

See `docs/development-roadmap.md` for the complete task list and progress tracking.

## Integration with Existing Design

This vision builds upon the existing design documents:
- **docs/rpdesign.md**: UI flow, room types, RPG mechanics
- **docs/pseudocode.md**: Technical implementation notes for C++ conversion

The popup system described here will be used for all the confirmation dialogs and text interactions described in the RPG design document, providing a consistent user interface throughout the game.

## Dungeon Crawler Scene Design

### UI Layout
- **Floor number**: Displayed at top middle of screen
- **BACK button**: Top left corner (returns to main menu)
- **INVENTORY button**: Top right corner
- **Grid display**: 5x5 black squares with white borders, connected by white lines
- **Player indicator**: Small green box showing current position on grid
- **Text box**: Bottom of screen for game prompts and narrative

### Movement System
The player navigates using directional buttons in the text box:
- **North, East, South, West** buttons
- Movement is turn-based and grid-based
- Position is tracked in temp_data (x_pos, y_pos)

### Room Types and Interactions

**Clear Rooms:**
- Text: "This room is clear. Where will you go?"
- Shows directional movement buttons
- No special interactions

**Wall Collision:**
- Text: "A wall stops your path."
- Shows "Continue" button to return to movement selection

**Treasure Rooms:**
- Initial prompt: "You sense a radiant presence beyond this door. Continue?"
- Buttons: "Yes" / "No"
- If Yes: "A chest lays in the center of the room. You open it and receive [ITEM]."
- Loot pool varies based on current floor number
- After looting, returns to movement selection

**Enemy Rooms:**
- Initial prompt: "You sense a dangerous presence beyond this door. Continue?"
- Buttons: "Yes" / "No"
- If No: Returns to movement selection
- If Yes: Fade to black, update position, transition to battle scene

**Boss Rooms:**
- Initial prompt: "You sense an ominous presence beyond this door. Continue?"
- Buttons: "Yes" / "No"
- If No: Returns to movement selection
- If Yes: Fade to black, update position, transition to boss battle scene

### State Management System
The scene uses a state machine approach:

**Initialization:**
- Load x_pos, y_pos from temp_data
- Identify current room type
- Set appropriate initial state

**State Flow:**
1. **Movement State**: Display "Where will you go?" with directional buttons
2. **Confirmation State**: Room-specific prompts with Yes/No or Continue options
3. **Transition State**: Handle scene changes or return to movement

**Button Handling:**
- Each directional button calculates proposed_x_pos, proposed_y_pos
- System validates movement (boundary checking)
- Room identification determines next state and text prompts
- All confirmations and prompts use the universal popup system

### Integration with Popup System
All room interactions use the popup system:
- Room detection prompts (radiant/dangerous/ominous presence)
- Treasure acquisition notifications
- Wall collision feedback
- Confirmation dialogs before entering dangerous areas

## Battle Encounter Scene Design

Based on the encounter system from your Soulseer project (Python/pygame), here's how we can adapt it for the Searching game:

### Core Architecture
- **Scene-based**: Battle encounter inherits from base scene system
- **Element system**: All battle objects (units, actions, hitboxes) are elements that update/draw
- **Y-order rendering**: Elements drawn based on their y-position for proper depth

## Unit System Architecture

The Unit class is the foundation for all combat entities, handling movement, animations, targeting, and action execution.

### Core Unit Properties
- **Health System**: HP/Max HP with visual healthbars (green overlay on red background)
- **Position and Physics**: 2D position, movement vectors, knockback handling
- **Side Logic**: Team identification (player side=0, enemy side=1)
- **Facing Direction**: Left/right orientation with sprite flipping
- **Stats**: Attack damage, movement speed, and other combat attributes

### Movement System
- **Target Location**: Units can be ordered to move to specific coordinates
- **Pathfinding**: Direct movement toward target location with collision avoidance
- **Movement Cancellation**: Moving cancels auto-attacks and resets cooldowns
- **Speed Control**: Configurable movement speed per unit type
- **Knockback Physics**: Temporary movement from taking damage or abilities

### Animation Management System
- **Default Animations**: Idle, move, death, hit animations stored in Unit
- **Animation Selection Logic**: 
  - Idle: Default state when no action or movement
  - Move: Plays during movement to target location
  - Death: Plays when HP reaches 0, non-repeating
  - Hit: Brief animation when taking damage
- **Action Override**: Current action's animation overrides default animations
- **Animation Transitions**: Smooth switching between animation states
- **Sprite Flipping**: Automatic sprite flipping based on facing direction

### Targeting System
- **Target Unit**: Reference to enemy unit for auto-attacks and abilities
- **Target Location**: Coordinate destination for movement commands
- **Range Calculation**: Distance checking for attack range validation
- **Target Acquisition**: Methods to set and clear targets
- **Target Persistence**: Maintains target through movement and actions

### Action Handling System
- **Action Storage**: Units store available actions (auto-attack + skills)
- **Action Execution**: 
  - Auto-attacks: Triggered when target is set and in range
  - Skills: Activated through input commands
  - Action Override: Only one action can execute at a time
- **Cooldown Management**: Tracks cooldown timers for all actions
- **Action Cancellation**: Movement cancels auto-attacks, skills prevent movement
- **Action Priority**: Skills interrupt auto-attacks, movement interrupts auto-attacks

### State Management
- **Combat States**: 
  - Idle: No action, no movement, no target
  - Moving: Traveling to target location
  - Auto-attacking: Executing auto-attack action
  - Casting: Executing skill action
  - Dead: HP at 0, death animation playing
- **State Transitions**: Rules for switching between states
- **State Validation**: Ensures legal state combinations

### Unit Subclasses

**Player Unit:**
- **Inheritance**: Inherits all Unit functionality
- **Input Handling**: Processes mouse clicks and keyboard input
- **Movement Commands**: Mouse click sets target location
- **Target Selection**: Mouse click on enemies sets target unit  
- **Skill Activation**: Keyboard keys (Q/W/E/R) trigger abilities
- **User Control**: Direct player control over all unit actions

**Enemy Unit:**
- **Inheritance**: Inherits all Unit functionality  
- **AI System**: enemy_AI() function replaces input handling
- **Autonomous Behavior**: AI makes all movement and combat decisions
- **Target Selection**: AI chooses targets based on proximity/threat
- **Action Selection**: AI decides when to auto-attack or use abilities
- **Behavioral Patterns**: Different AI logic per enemy type

### Action and Combat System (Warcraft 3-inspired)

**Unit Selection and Control:**
- Units can be selected like in Warcraft 3
- Selected units can be ordered to target and auto-attack enemies
- Auto-attack is a skill object with its own properties and behavior

**Action Class Architecture:**
All combat abilities inherit from a base Action class with:
- **Cooldown system**: Time before action can be used again
- **Activation time**: Duration the action is active/executing
- **Animation object**: Each Action stores its own Animation object
- **Animation override**: When Action executes, it overrides Unit's current Animation
- **Frame-based logic**: Action's update() function contains frame-specific logic
- **Dynamic timing**: Animation speed adjusts to match Action's activation time

**Animation Integration System:**
- **Unit animations**: Units have default animations (idle, move, etc.) that provide images each frame
- **Action animations**: Actions override Unit's current animation during execution
- **Frame synchronization**: Actions execute logic based on specific animation frames
- **Speed scaling**: Helper functions in Animation class adjust frame timing to fit activation duration
- **Logic timing**: Example pseudocode: `if (current_frame == x) { unit.target.take_damage(50 * unit.atk); }`

## Animation System Architecture

The Animation object is a core component that manages sprite animation and timing for both Units and Actions.

### Core Animation Properties
- **Spritesheet loading**: Loads sprite strips and divides into individual frames
- **Frame management**: Tracks current frame, frame count, and frame timing
- **Frame duration**: Configurable timing for each frame advance
- **Repeat behavior**: Can loop infinitely or play once
- **Image provision**: Provides current frame image for rendering

### Frame Timing and Speed Control
- **Base frame duration**: Default timing for normal animation speed
- **Dynamic speed adjustment**: Helper functions to scale animation to fit specific durations
- **Frame-to-logic mapping**: Functions to determine logical frame events regardless of speed
- **Synchronization**: Ensures animation frames align with Action activation timing

### Integration with Combat System
- **Action override**: Actions can replace Unit's current animation during execution
- **Event triggering**: Specific animation frames trigger combat logic (damage, effects, etc.)
- **Completion detection**: Animation system reports when animations finish
- **State management**: Tracks animation state for proper transitions back to idle/move states

### Spritesheet Specifications

**Player Character (Knight) Spritesheets:**
- **Frame dimensions**: 135x135 pixels per frame
- **Layout**: Horizontal strip format (frames arranged left to right)
- **Frame counts**:
  - Idle.png: 10 frames (1350x135 total)
  - Run.png: 6 frames (810x135 total)
  - Attack1.png: 4 frames (540x135 total)
  - Attack2.png: 4 frames (540x135 total) 
  - Attack3.png: 5 frames (675x135 total)
  - Get Hit.png: 3 frames (405x135 total)
  - Death.png: 9 frames (1215x135 total)

**Enemy Spritesheets:**
- **Skeleton**: 150x150 pixels per frame
  - Idle.png: 4 frames (600x150 total)
  - Walk.png: 4 frames (600x150 total)
  - Attack.png: 8 frames (1200x150 total)
  - Take Hit.png: 4 frames (600x150 total)
  - Death.png: 4 frames (600x150 total)

- **Goblin**: 150x150 pixels per frame
  - Idle.png: 4 frames (600x150 total)
  - Run.png: 4 frames (600x150 total)
  - Attack.png: 4 frames (600x150 total)
  - Take Hit.png: 4 frames (600x150 total)
  - Death.png: 4 frames (600x150 total)

- **Flying Eye**: 150x150 pixels per frame
  - Flight.png: 8 frames (1200x150 total)
  - Attack.png: 8 frames (1200x150 total)
  - Take Hit.png: 4 frames (600x150 total)
  - Death.png: 4 frames (600x150 total)

### Frame Extraction Algorithm
```cpp
// Pseudocode for loading spritesheet
Animation loadAnimation(string filepath, int frameCount, int frameSize) {
    Texture2D spritesheet = LoadTexture(filepath);
    
    for(int i = 0; i < frameCount; i++) {
        Rectangle sourceRect = {
            i * frameSize,  // x position
            0,              // y position  
            frameSize,      // width
            frameSize       // height
        };
        
        // Extract frame from spritesheet
        frames[i] = ImageFromTexture(spritesheet, sourceRect);
    }
}
```

### Technical Requirements
- **Frame extraction**: Parse spritesheets using frameSize and frameCount parameters
- **Dynamic sizing**: Support both 135x135 (player) and 150x150 (enemies) frame sizes
- **Timing helpers**: Functions to map logical events to actual animation frames
- **Speed scaling**: Adjust frame duration to fit Action activation times
- **Reset functionality**: Return animations to initial state when needed

**Auto Attack System:**
- **Type**: Melee auto-attack skill (inherits from Action class)
- **Targeting**: Requires enemy target to initiate
- **Range check**: During one specific frame of activation, checks if target is in range
- **Damage dealing**: Calls target's take_damage() function with damage amount
- **Damage calculation**: Done on victim side, returns actual damage dealt to attacker
- **Movement cancellation**: If unit moves during activation, auto-attack is cancelled and cooldown resets
- **Movement restriction**: Moving always resets auto-attack cooldown

**Skill System:**
- **Inheritance**: Skills inherit from same Action class as auto-attacks
- **Activation**: Instant activation (no wind-up time)
- **Interaction**: Skills cancel any ongoing auto-attacks
- **Movement restriction**: Units cannot move during skill duration
- **Priority**: Skills take precedence over auto-attacks

**Action States:**
1. **Cooldown**: Action unavailable, cooling down
2. **Ready**: Action available for use
3. **Activating**: Action executing, animation playing, effects can trigger
4. **Complete**: Action finished, returns to cooldown state

### UI and Visual Elements
**Action Bar:**
- Top-left corner, horizontal layout
- 100x100 squares with 10px spacing
- White borders around ability icons
- Cooldown overlay (top-to-bottom gray fill)

**Healthbars:**
- Above each unit (5px height, unit width)
- Red background, green foreground based on HP ratio
- Hidden when unit is dead

**Visual Effects:**
- Sprite outlines for unit visibility
- Y-order depth sorting for proper layering
- Smooth animation system with configurable frame rates

### Game Flow
1. **Initialization**: Spawn player unit, position enemies randomly off-screen sides
2. **Main Loop**: Update all elements, handle input, check win conditions
3. **Victory Conditions**: Battle ends when all units of one side are defeated
4. **Transition**: Fade back to dungeon crawler scene after victory delay

### Adaptation for Raylib/C++
**Key Changes Needed:**
- Convert pygame Rectangle/Vector2 to raylib equivalents
- Adapt sprite loading and animation system to raylib textures
- Replace pygame event system with raylib input handling
- Convert Python classes to C++ classes with proper memory management
- Adapt collision detection to raylib's collision functions

**Architecture Integration:**
- Battle scene fits into the scene management system
- Uses universal popup system for battle results
- Integrates with save/load system for character progression
- Fade transitions in/out of battle matches other scene transitions

## Equipment and Effects System

### Equipment Integration
**Equipment Loading Process:**
- Equipment items are loaded by name from save files (e.g., "IRON_HELMET", "LEATHER_ARMOR")
- Equipment names correspond to entries in item database files
- Equipment is not instantiated as objects - instead provides stat bonuses and effects

**Battle Scene Equipment Loading:**
1. Read equipped items from `char_1/items.txt`
2. Look up each item name in item database
3. Apply flat stat bonuses to Unit (ATK+5, DEF+2, etc.)
4. Apply permanent Effects to Unit
5. Effects remain active throughout battle

**Inventory Scene Equipment Loading:**
- Display available equipment from `inv_items.txt`
- Show equipment stats and effects for comparison
- Allow equipping/unequipping with immediate stat preview

### Effects System Architecture

**Effects Base Class Structure:**
```cpp
class Effect {
public:
    bool remove = false;           // Marked for removal flag
    int duration = -1;             // -1 = permanent, >0 = temporary
    
    virtual void onAdd(Unit* unit) = 0;      // Apply initial effect
    virtual void update(Unit* unit) = 0;     // Called each frame
    virtual void onRemove(Unit* unit) = 0;   // Cleanup when removed
};
```

**Effect Lifecycle:**
1. **Add Logic** (`onAdd`): Apply immediate changes (e.g., +100 max_hp, +100 hp)
2. **Update Logic** (`update`): Called every frame, handles duration countdown and ongoing effects
3. **Remove Logic** (`onRemove`): Cleanup/reverse changes when effect expires

**Example - Temporary Health Bonus Effect:**
```cpp
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
```

**Memory Management Strategy:**
```cpp
// In Unit class
std::vector<std::unique_ptr<Effect>> effects;

// Adding effects
void addEffect(std::unique_ptr<Effect> effect) {
    effect->onAdd(this);
    effects.push_back(std::move(effect));
}

// Update and cleanup (called each frame)
void updateEffects() {
    // Update all effects
    for (auto& effect : effects) {
        effect->update(this);
    }
    
    // Remove marked effects (C++20 std::erase_if method)
    std::erase_if(effects, [this](const std::unique_ptr<Effect>& effect) {
        if (effect->remove) {
            effect->onRemove(this);
            return true;
        }
        return false;
    });
}
```

**Equipment Effects (Permanent):**
- Equipment effects have `duration = -1` (never expire)
- `onRemove` only called when equipment is unequipped
- Examples: +ATK bonus, poison immunity, regeneration

**Immunity System Implementation:**
```cpp
// In Unit class - immunity flags
bool poisonImmune = false;
bool stunImmune = false;
bool fireImmune = false;
bool freezeImmune = false;
// Add more immunity types as needed

// Poison Immunity Effect
class PoisonImmunityEffect : public Effect {
public:
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

// When applying poison effects, check immunity
void applyPoisonEffect(Unit* target) {
    if (!target->poisonImmune) {
        target->addEffect(std::make_unique<PoisonEffect>(damage, duration));
    }
    // If immune, poison effect is blocked
}
```

**Scaling the Immunity System:**
- Add new immunity booleans to Unit class as needed
- Each immunity effect toggles its specific boolean
- Effect application functions check relevant immunity flags
- Simple, direct, and easy to understand
- Performance-efficient with direct boolean checks

### Item Database Structure
**Separate data files will define:**
- Item stats (ATK bonus, DEF bonus, HP bonus)
- Item effects (regeneration, critical hit chance, elemental resistance)
- Item requirements (level restrictions, class restrictions)
- Item descriptions and display information

## Next Steps

Continue elaborating on:
- Asset requirements and file structure
- Character progression and equipment system
- Effects system implementation details