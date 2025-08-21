# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ dungeon crawler game called "Searching" using the raylib graphics library. The game features a grid-based exploration system with rooms, enemies, treasures, and RPG-style character progression.

## Build System

The project uses CMake for cross-platform building:

**Build commands:**
```bash
# Configure and build
cmake -B build
cmake --build build

# Run the game
cmake --build build --target run
# Or directly: ./build/Searching-game
```

The CMakeLists.txt automatically:
- Links against the raylib library in `lib/`
- Includes necessary macOS frameworks when building on Apple platforms
- Provides a custom `run` target for easy execution
- Recursively finds all C/C++ source files in `src/`

## Architecture

- **Language**: C++
- **Graphics Library**: raylib
- **Project Structure**:
  - `src/` - Source code organized by class/system
    - `main.cpp` - Entry point with basic raylib window initialization
    - `Game.cpp` - Main game loop and scene management
    - `Scene.cpp` - Base scene class
    - `MainMenuScene.cpp`, `DungeonScene.cpp`, `BattleScene.cpp` - Scene implementations
    - `Popup.cpp` - Universal popup system
    - `Animation.cpp` - Sprite animation system
    - `Unit.cpp`, `Player.cpp`, `Enemy.cpp` - Combat entity classes
    - `Action.cpp` - Combat action system
    - `Utils.cpp` - Helper functions and file I/O
  - `lib/` - Contains raylib static library and headers
    - `libraylib.a` - Compiled raylib library for linking
    - `raylib.h` - Main raylib API header
    - `raymath.h` - Math utilities header
  - `docs/` - Design documentation
    - `pseudocode.md` - Technical implementation notes for C++ conversion
    - `rpdesign.md` - Game design document with UI flow and mechanics
    - `game-vision.md` - Current game architecture and detailed vision
    - `development-roadmap.md` - Task tracking and development priorities
    - `implementation-guide.md` - Specific code examples and implementation details
    - `current-status.md` - Project status and session handoff information
    - `image.png` - Visual reference
  - `build/` - CMake build directory (auto-generated)
  - `assets/` - Game assets organized by category
    - `player/` - Player character (Knight) animations
    - `enemies/` - Enemy animations organized by type (skeleton, goblin, mushroom, flying_eye)
    - `abilities/` - Ability icons and visual effects

## Game Design

Based on `docs/rpdesign.md`, the game features:

- **Main Menu**: Play, Save, Load buttons
- **Grid-based Exploration**: 5x5 room grid with player movement
- **Room Types**: Clear rooms, treasure rooms, enemy rooms, boss rooms
- **RPG Elements**: Character stats (HP, ATK, DEF, LVL), equipment system
- **Save System**: File-based character and progress persistence in `temp_data/`

Key UI elements:
- Floor indicator (top middle)
- Back button (top left)
- Inventory button (top right)
- Text box with contextual prompts and directional movement buttons

## Current State

The project contains a minimal raylib example that:
- Creates an 800x450 window titled "Searching"
- Runs at 60 FPS
- Uses manually defined Color structs instead of raylib color macros
- Has placeholder content ready for game implementation

## Development Notes

- The project uses `extern "C"` wrapper for raylib headers to maintain C++ compatibility
- Cannot use raylib color macros (like RAYWHITE) - must define Color structs manually
- Game logic should follow the state machine pattern outlined in `docs/rpdesign.md`
- Save data structure is defined with character stats, inventory, and progress tracking