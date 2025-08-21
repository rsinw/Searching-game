# Source Code Organization

This directory contains all C++ source files for the Searching dungeon crawler game.

## File Structure

### Core System Files
- **`main.cpp`** - Entry point, initializes game and starts main loop
- **`Game.cpp`** - Main game loop, window management, scene switching
- **`Scene.cpp`** - Base scene class that other scenes inherit from
- **`Utils.cpp`** - Helper functions, file I/O, save/load system

### Scene Implementation Files
- **`MainMenuScene.cpp`** - Main menu with PLAY, LOAD, SAVE, EXIT buttons
- **`DungeonScene.cpp`** - Dungeon crawler scene with grid exploration
- **`BattleScene.cpp`** - Battle encounter scene with Warcraft 3-inspired combat

### UI System Files
- **`Popup.cpp`** - Universal popup system for confirmations and dialogs

### Graphics and Animation Files
- **`Animation.cpp`** - Sprite animation system, frame management, timing

### Combat System Files
- **`Unit.cpp`** - Base unit class (health, movement, animations, targeting)
- **`Player.cpp`** - Player unit class (input handling, user control)
- **`Enemy.cpp`** - Enemy unit class (AI behavior, autonomous actions)
- **`Action.cpp`** - Base action class + auto-attacks and skills
- **`Effects.cpp`** - Equipment and ability effects system

## Class Hierarchy

```
Scene (base)
├── MainMenuScene
├── DungeonScene  
└── BattleScene

Unit (base)
├── Player
└── Enemy

Action (base)
├── AutoAttack
├── Skill1
├── Skill2
└── Skill3
```

## Build Integration

All .cpp files are automatically included in the CMake build through:
```cmake
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
```

## Development Notes

- No header files used - forward declarations at top of files when needed
- Each class in its own file for clean organization
- Related functionality grouped logically
- Inheritance relationships clearly defined