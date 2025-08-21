# Development Roadmap

This document tracks the development tasks for the "Searching" dungeon crawler game.

**Note**: See `docs/implementation-guide.md` for specific code examples and implementation details when ready to implement these systems.

## Core Systems (Priority 1)
- [ ] Scene management system for switching between screens
- [ ] Fade-to-black transition system between scenes
- [ ] Popup system that can be displayed over any scene
- [ ] Popup visual design - white bordered rectangle
- [ ] Popup text typing animation with enter-to-skip functionality
- [ ] Popup button system with customizable actions
- [ ] Popup continue prompt (>) when no buttons present

## Main Menu Implementation (Priority 2)
- [ ] Main menu scene with PLAY, LOAD, SAVE, and EXIT buttons
- [ ] PLAY button functionality - load temp_data and go to dungeon crawler scene
- [ ] LOAD button functionality - show save file selection list
- [ ] SAVE button functionality - save temp_data to file with confirmation popup
- [ ] EXIT button functionality - quit game with confirmation popup

## Game Scenes (Priority 3)
- [ ] Dungeon crawler scene implementation (based on rpdesign.md)

### Dungeon Crawler Scene Components
- [ ] UI Layout: Floor number (top middle), BACK button (top left), INVENTORY button (top right)
- [ ] 5x5 grid display: Black squares with white borders, connected by white lines
- [ ] Player position indicator: Small green box on the grid
- [ ] Text box at bottom for game prompts and narrative
- [ ] Directional movement system: North, East, South, West buttons
- [ ] Room identification system: Clear, treasure, enemy, boss room types
- [ ] Movement validation: Wall collision detection and feedback
- [ ] Room interaction prompts using popup system
- [ ] Treasure room mechanics: Radiant presence detection, loot distribution by floor
- [ ] Enemy room mechanics: Dangerous presence detection, battle transition
- [ ] Boss room mechanics: Ominous presence detection, boss battle transition
- [ ] State management system for room exploration flow
- [ ] Position tracking and persistence in temp_data

## Battle Encounter Scene (Priority 4)
- [ ] Battle encounter scene implementation (adapted from Soulseer project)

### Core Battle System Components
- [ ] Element management system for battle objects (units, actions, hitboxes)
- [ ] Y-order rendering system for proper depth layering
- [ ] Unit base class with HP, movement, animations, and side logic
- [ ] Player unit with mouse movement/targeting and keyboard abilities (Q/W/E/R)
- [ ] Enemy unit AI with targeting and auto-attack behavior
- [ ] Animation system supporting idle, move, attack, death, and hit states
- [ ] Action system with cooldowns and visual ability icons
- [ ] Hitbox system with collision detection and damage dealing
- [ ] Healthbar rendering (red background, green overlay)
- [ ] Action bar UI (top-left, 100x100 icons with cooldown overlays)
- [ ] Victory condition detection and battle end handling
- [ ] Integration with scene transition system (fade in/out)

### Adaptation from Python/Pygame to C++/Raylib
- [ ] Convert pygame Rectangle/Vector2 to raylib equivalents
- [ ] Adapt sprite loading and animation to raylib textures
- [ ] Replace pygame event system with raylib input handling
- [ ] Convert Python classes to C++ with proper memory management
- [ ] Adapt collision detection to raylib collision functions

## Future Tasks (To Be Detailed)
- [ ] Implement file I/O functions in Utils.cpp for save/load system (see implementation-guide.md)
- [ ] Create Effects system for equipment and abilities (see implementation-guide.md)
- [ ] Design and implement Animation system architecture (see implementation-guide.md)
- [ ] Create item database for equipment stats and effects
- [ ] RPG combat mechanics
- [ ] Inventory system
- [ ] Character progression
- [ ] Room generation and exploration
- [ ] Audio system
- [ ] Game balancing and polish