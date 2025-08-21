# Assets Organization

This directory contains all game assets organized by category for the Searching dungeon crawler game.

## Directory Structure

### `/player/`
Player character (Knight) animations:
- `Idle.png` - Standing idle animation (10 frames)
- `Run.png` - Movement animation (6 frames) 
- `Attack1.png` - Basic attack animation (4 frames)
- `Attack2.png` - Second attack animation (4 frames)
- `Attack3.png` - Third attack animation (5 frames)
- `Get Hit.png` - Damage taken animation (3 frames)
- `Death.png` - Death animation (9 frames, non-repeating)
- `Jump.png` - Jump animation (if needed)
- `Fall.png` - Fall animation (if needed)

### `/abilities/`
Ability icons and visual effects:
- `slash1.png` - Icon for basic attack ability
- `slash2.png` - Icon for second attack ability  
- `slash3.png` - Icon for third attack ability

### `/enemies/`
Enemy character animations organized by type:

#### `/enemies/skeleton/`
- `Idle.png` - Skeleton idle animation (4 frames)
- `Walk.png` - Skeleton movement animation (4 frames)
- `Attack.png` - Skeleton attack animation (8 frames)
- `Take Hit.png` - Skeleton damage animation (4 frames)
- `Death.png` - Skeleton death animation (4 frames)
- `Shield.png` - Skeleton shield/block animation (if applicable)

#### `/enemies/goblin/`
- `Idle.png` - Goblin idle animation
- `Run.png` - Goblin movement animation
- `Attack.png` - Goblin attack animation
- `Take Hit.png` - Goblin damage animation
- `Death.png` - Goblin death animation

#### `/enemies/mushroom/`
- `Idle.png` - Mushroom idle animation
- `Run.png` - Mushroom movement animation
- `Attack.png` - Mushroom attack animation
- `Take Hit.png` - Mushroom damage animation
- `Death.png` - Mushroom death animation

#### `/enemies/flying_eye/`
- `Flight.png` - Flying eye movement/idle animation
- `Attack.png` - Flying eye attack animation
- `Take Hit.png` - Flying eye damage animation
- `Death.png` - Flying eye death animation

## Animation Specifications

- **Frame Size**: Most sprites use 135x135 or 150x150 pixel frames
- **Format**: PNG sprite sheets with horizontal frame layout
- **Naming**: Consistent naming convention for easy loading
- **Frame Counts**: Vary by animation type (noted above)

## Usage Notes

- All animations are designed for the raylib/C++ implementation
- Frame counts and timing will be configurable through the Animation system
- Sprite sheets should be loaded and divided into individual frames
- Animation speeds will be adjustable to fit Action activation times