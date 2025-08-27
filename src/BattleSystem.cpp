extern "C" {
    #include <raylib.h>
    #include <raymath.h>
}

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <typeinfo>

// ==================== ECS ARCHITECTURE ====================

// Entity: Just a unique ID
using Entity = int;

// Component base class
class Component {
public:
    virtual ~Component() = default;
};

// ==================== COMPONENTS ====================

class PositionComponent : public Component {
public:
    float x, y;
    bool facing_right;
    Rectangle rect;
    
    PositionComponent(float x, float y, float w = 50, float h = 50) 
        : x(x), y(y), facing_right(true) {
        rect = {x, y, w, h};
    }
    
    void update_rect() {
        rect.x = x;
        rect.y = y;
    }
    
    // Get center-bottom position like backup system
    Vector2 get_center_bottom() {
        return {rect.x + rect.width/2, rect.y + rect.height};
    }
    
    // Set position from center-bottom coordinates 
    void set_from_center_bottom(float center_x, float bottom_y) {
        rect.x = center_x - rect.width/2;
        rect.y = bottom_y - rect.height;
        x = rect.x;
        y = rect.y;
    }
};

class HealthComponent : public Component {
public:
    int hp, max_hp;
    bool is_dead;
    int remove_timer;
    
    HealthComponent(int max_hp) : hp(max_hp), max_hp(max_hp), is_dead(false), remove_timer(0) {}
    
    void take_damage(int damage) {
        hp -= damage;
        if (hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }
};

class MovementComponent : public Component {
public:
    float move_dx, move_dy;
    float speed;
    float knockback_dx, knockback_dy;
    
    MovementComponent(float speed = 2.0f) 
        : move_dx(0), move_dy(0), speed(speed), knockback_dx(0), knockback_dy(0) {}
};

// Animation helper class
class Animation {
public:
    Texture2D spritesheet;
    Rectangle frameRec;        // Rectangle for current frame (proper raylib approach from backup)
    int num_frames;
    int current_frame;
    int frame_duration;
    int frame_timer;
    int frame_width, frame_height;  // Changed to int for consistency with backup
    bool repeat;
    
    Animation(const std::string& path, int frames, int frame_size = 135) {
        spritesheet = LoadTexture(path.c_str());
        num_frames = frames;
        current_frame = 0;
        frame_duration = 10;  // Match backup frame duration
        frame_timer = 0;
        repeat = true;
        
        if (spritesheet.id != 0) {
            frame_width = spritesheet.width / num_frames;
            frame_height = spritesheet.height;
            std::cout << "Animation loaded: " << path << " (" << frames << " frames)" << std::endl;
            std::cout << "  Frame size: " << frame_width << "x" << frame_height << std::endl;
        } else {
            frame_width = frame_size;
            frame_height = frame_size;
            std::cout << "Failed to load: " << path << std::endl;
        }
        
        // Initialize frame rectangle for first frame (from backup)
        frameRec = { 0.0f, 0.0f, (float)frame_width, (float)frame_height };
    }
    
    ~Animation() {
        if (spritesheet.id != 0) {
            UnloadTexture(spritesheet);
        }
    }
    
    void update() {
        frame_timer += 1;
        
        if (frame_timer >= frame_duration) {
            frame_timer = 0;
            current_frame += 1;
            if (current_frame >= num_frames) {
                if (repeat) {
                    current_frame = 0;
                } else {
                    current_frame = num_frames - 1;
                }
            }
        }
        
        // Update frame rectangle to show current frame (from backup)
        frameRec.x = (float)(current_frame * frame_width);
    }
    
    void reset() {
        current_frame = 0;
        frame_timer = 0;
        frameRec.x = 0.0f;
    }
    
    // Draw method using proper raylib DrawTextureRec (from backup)
    void draw(Vector2 position, bool facing_right = true, float scale = 2.0f) {
        if (spritesheet.id == 0) return;  // No texture loaded
        
        Rectangle destRec = { position.x, position.y, frame_width * scale, frame_height * scale };
        Rectangle sourceRec = frameRec;
        
        // Flip horizontally if facing left (from backup)
        if (!facing_right) {
            sourceRec.width = -sourceRec.width;
        }
        
        DrawTexturePro(spritesheet, sourceRec, destRec, {0, 0}, 0.0f, WHITE);
    }
};

class AnimationComponent : public Component {
public:
    std::unique_ptr<Animation> idle_anim;
    std::unique_ptr<Animation> move_anim;
    std::unique_ptr<Animation> attack_anim;
    std::unique_ptr<Animation> hit_anim;
    std::unique_ptr<Animation> death_anim;
    Animation* current_anim;
    float offsetx, offsety;  // Proper sprite positioning offsets from backup
    int scale;
    
    // Different offsets for different sprite types
    void set_player_offsets() {
        offsetx = -50.0f * scale; // Player sprite offset
        offsety = -40.0f * scale;
    }
    
    void set_enemy_offsets() {
        offsetx = -60.0f * scale; // Enemy sprites might need different offset
        offsety = -50.0f * scale;
    }
    
    AnimationComponent() : current_anim(nullptr), scale(2) {
        // Proper offset calculations from backup: -50*scale, -40*scale
        offsetx = -50.0f * scale; // -100
        offsety = -40.0f * scale; // -80
    }
    
    // Proper switch_anim method from backup to prevent redundant changes
    void switch_anim(Animation* new_anim) {
        if (current_anim == new_anim) return;  // More strict check from backup
        current_anim = new_anim;
        if (current_anim) {
            current_anim->reset();
        }
    }
    
    void update() {
        if (current_anim) {
            current_anim->update();
        }
    }
    
    void draw(Vector2 position, bool facing_right) {
        if (current_anim) {
            // Use proper offset positioning from backup
            Vector2 draw_pos = { position.x + offsetx, position.y + offsety };
            current_anim->draw(draw_pos, facing_right, scale);
        }
    }
};

class AttackComponent : public Component {
public:
    int cooldown;
    int cooldown_timer;
    int damage;
    float range;
    int duration;
    int duration_timer;
    int swing_frame;
    bool is_attacking;
    
    AttackComponent(int cd = 60, int dmg = 10, float rng = 120, int dur = 30, int swing = 15) 
        : cooldown(cd), cooldown_timer(0), damage(dmg), range(rng), 
          duration(dur), duration_timer(0), swing_frame(swing), is_attacking(false) {}
    
    bool can_attack() {
        return cooldown_timer <= 0 && !is_attacking;
    }
    
    void start_attack() {
        is_attacking = true;
        duration_timer = 0;
    }
    
    void update_attack() {
        if (is_attacking) {
            duration_timer++;
            if (duration_timer >= duration) {
                is_attacking = false;
                duration_timer = 0;
            }
        }
        if (cooldown_timer > 0) {
            cooldown_timer--;
        }
    }
    
    void cancel_attack() {
        is_attacking = false;
        duration_timer = 0;
        cooldown_timer = 0; // Remove cooldown on cancel as specified
    }
    
    void start_cooldown() {
        cooldown_timer = cooldown;  // Start cooldown on swing as in backup
    }
};

class AIComponent : public Component {
public:
    int side; // 0 = player, 1 = enemy
    Entity target_entity;
    bool has_target;
    bool has_move_target; // Flag for movement target (like backup system)
    Vector2 move_target; // Target location for movement
    std::string type_name; // For debugging
    
    AIComponent(int side, const std::string& name = "") : side(side), target_entity(-1), has_target(false), has_move_target(false), move_target({0,0}), type_name(name) {}
};

// ==================== ENTITY COMPONENT SYSTEM ====================

class ECS {
private:
    int next_entity_id = 0;
    std::unordered_map<Entity, std::unordered_map<size_t, std::unique_ptr<Component>>> components;
    
public:
    Entity create_entity() {
        return next_entity_id++;
    }
    
    template<typename T, typename... Args>
    void add_component(Entity entity, Args&&... args) {
        size_t type_id = typeid(T).hash_code();
        components[entity][type_id] = std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    T* get_component(Entity entity) {
        size_t type_id = typeid(T).hash_code();
        auto entity_it = components.find(entity);
        if (entity_it != components.end()) {
            auto comp_it = entity_it->second.find(type_id);
            if (comp_it != entity_it->second.end()) {
                return static_cast<T*>(comp_it->second.get());
            }
        }
        return nullptr;
    }
    
    template<typename T>
    std::vector<Entity> get_entities_with() {
        std::vector<Entity> result;
        size_t type_id = typeid(T).hash_code();
        
        for (auto& [entity, comp_map] : components) {
            if (comp_map.find(type_id) != comp_map.end()) {
                result.push_back(entity);
            }
        }
        return result;
    }
    
    template<typename T1, typename T2>
    std::vector<Entity> get_entities_with() {
        std::vector<Entity> result;
        size_t type_id1 = typeid(T1).hash_code();
        size_t type_id2 = typeid(T2).hash_code();
        
        for (auto& [entity, comp_map] : components) {
            if (comp_map.find(type_id1) != comp_map.end() &&
                comp_map.find(type_id2) != comp_map.end()) {
                result.push_back(entity);
            }
        }
        return result;
    }
    
    template<typename T1, typename T2, typename T3>
    std::vector<Entity> get_entities_with() {
        std::vector<Entity> result;
        size_t type_id1 = typeid(T1).hash_code();
        size_t type_id2 = typeid(T2).hash_code();
        size_t type_id3 = typeid(T3).hash_code();
        
        for (auto& [entity, comp_map] : components) {
            if (comp_map.find(type_id1) != comp_map.end() &&
                comp_map.find(type_id2) != comp_map.end() &&
                comp_map.find(type_id3) != comp_map.end()) {
                result.push_back(entity);
            }
        }
        return result;
    }
    
    void remove_entity(Entity entity) {
        components.erase(entity);
    }
    
    std::vector<Entity> get_all_entities() {
        std::vector<Entity> result;
        for (auto& [entity, comp_map] : components) {
            result.push_back(entity);
        }
        return result;
    }
};

// ==================== SYSTEMS ====================

class MovementSystem {
public:
    void update(ECS& ecs) {
        auto entities = ecs.get_entities_with<PositionComponent, MovementComponent>();
        
        for (Entity entity : entities) {
            auto* pos = ecs.get_component<PositionComponent>(entity);
            auto* mov = ecs.get_component<MovementComponent>(entity);
            
            // Update position
            pos->x += mov->move_dx;
            pos->y += mov->move_dy;
            
            // Apply knockback using center-bottom logic
            if (mov->knockback_dx != 0 || mov->knockback_dy != 0) {
                Vector2 current = pos->get_center_bottom();
                Vector2 knockback_pos = {current.x + mov->knockback_dx, current.y + mov->knockback_dy};
                pos->set_from_center_bottom(knockback_pos.x, knockback_pos.y);
                
                mov->knockback_dx *= 0.9f;
                mov->knockback_dy *= 0.9f;
                if (abs(mov->knockback_dx) < 0.1f) mov->knockback_dx = 0;
                if (abs(mov->knockback_dy) < 0.1f) mov->knockback_dy = 0;
            }
            
            // Update facing direction
            if (mov->move_dx > 0) pos->facing_right = true;
            else if (mov->move_dx < 0) pos->facing_right = false;
            
            pos->update_rect();
        }
    }
};

class AnimationSystem {
public:
    void update(ECS& ecs) {
        auto entities = ecs.get_entities_with<AnimationComponent>();
        
        for (Entity entity : entities) {
            auto* anim = ecs.get_component<AnimationComponent>(entity);
            if (anim->current_anim) {
                anim->current_anim->update();
            }
        }
    }
    
    void render(ECS& ecs) {
        auto entities = ecs.get_entities_with<PositionComponent, AnimationComponent>();
        
        for (Entity entity : entities) {
            auto* pos = ecs.get_component<PositionComponent>(entity);
            auto* anim = ecs.get_component<AnimationComponent>(entity);
            
            if (anim->current_anim && anim->current_anim->spritesheet.id != 0) {
                // Use the draw method from Animation class which handles proper positioning
                Vector2 draw_pos = {pos->x, pos->y};
                anim->draw(draw_pos, pos->facing_right);
            }
        }
    }
};

class AttackSystem {
public:
    void update(ECS& ecs) {
        auto entities = ecs.get_entities_with<PositionComponent, AttackComponent, AIComponent>();
        
        for (Entity entity : entities) {
            auto* pos = ecs.get_component<PositionComponent>(entity);
            auto* attack = ecs.get_component<AttackComponent>(entity);
            auto* ai = ecs.get_component<AIComponent>(entity);
            auto* mov = ecs.get_component<MovementComponent>(entity);
            auto* anim = ecs.get_component<AnimationComponent>(entity);
            
            // Update attack timers
            attack->update_attack();
            
            // Your core logic implementation (the fundamental flow)
            execute_core_logic(ecs, entity, pos, attack, ai, mov, anim);
        }
    }
    
private:
    void execute_core_logic(ECS& ecs, Entity entity, PositionComponent* pos, 
                          AttackComponent* attack, AIComponent* ai, 
                          MovementComponent* mov, AnimationComponent* anim) {
        
        // Critical fix from backup: Dead units should not perform any actions
        auto* health = ecs.get_component<HealthComponent>(entity);
        if (health && health->is_dead) {
            // Clear any current attack for dead units (critical fix from backup)
            if (attack) {
                attack->is_attacking = false;
                attack->duration_timer = 0;
            }
            // Clear movement
            if (mov) {
                mov->move_dx = 0;
                mov->move_dy = 0;
            }
            // Force death animation
            if (anim && anim->death_anim) {
                anim->switch_anim(anim->death_anim.get());
            }
            return;  // Skip all other processing for dead units
        }
        
        // Enemy AI - find player target
        if (ai->side == 1) {
            find_closest_target(ecs, entity, pos, ai, 0); // Find player targets
        }
        
        // Core flow: check for movement target first (like backup system)
        if (!ai->has_target && ai->has_move_target) {
            // Movement to location logic (like backup system)
            Vector2 current = pos->get_center_bottom();
            Vector2 target = ai->move_target;
            Vector2 direction = {target.x - current.x, target.y - current.y};
            float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
            
            if (distance <= mov->speed) {
                // Reached target - stop moving
                pos->set_from_center_bottom(target.x, target.y);
                ai->has_move_target = false;
                mov->move_dx = 0;
                mov->move_dy = 0;
                if (anim && anim->idle_anim) {
                    anim->switch_anim(anim->idle_anim.get());
                }
            } else {
                // Move towards target
                direction.x /= distance;
                direction.y /= distance;
                mov->move_dx = direction.x * mov->speed;
                mov->move_dy = direction.y * mov->speed;
                pos->facing_right = (direction.x > 0);
                if (anim && anim->move_anim) {
                    anim->switch_anim(anim->move_anim.get());
                }
            }
            return;
        }
        
        // Core flow: no target -> idle
        if (!ai->has_target) {
            if (mov) {
                mov->move_dx = 0;
                mov->move_dy = 0;
            }
            if (anim && anim->idle_anim) {
                anim->switch_anim(anim->idle_anim.get());
            }
            return;
        }
        
        // Get target position and health
        auto* target_pos = ecs.get_component<PositionComponent>(ai->target_entity);
        auto* target_health = ecs.get_component<HealthComponent>(ai->target_entity);
        
        if (!target_pos || !target_health || target_health->is_dead) {
            ai->has_target = false;
            return;
        }
        
        // Calculate edge-to-edge distance for more forgiving range
        Vector2 current_pos = pos->get_center_bottom();
        Vector2 target_pos_cb = target_pos->get_center_bottom();
        
        // Calculate closest edge distance considering sprite sizes
        float sprite_buffer = 80.0f; // Combined sprite width buffer for melee range
        float dx = target_pos_cb.x - current_pos.x;
        float dy = target_pos_cb.y - current_pos.y;
        float center_distance = sqrt(dx*dx + dy*dy);
        float distance = fmax(0, center_distance - sprite_buffer); // More forgiving range
        
        // During auto attack - check range and target death, cancel if needed
        if (attack->is_attacking) {
            if (distance > attack->range) {
                // Out of range - cancel and remove cooldown
                attack->cancel_attack();
                if (anim && anim->idle_anim) {
                    anim->switch_anim(anim->idle_anim.get());
                }
            } else if (target_health->is_dead) {
                // Target died during attack - cancel and go idle
                attack->cancel_attack();
                if (anim && anim->idle_anim) {
                    anim->switch_anim(anim->idle_anim.get());
                }
                std::cout << ai->type_name << " stops attacking - target is dead" << std::endl;
            } else {
                // Deal damage at swing frame
                if (attack->duration_timer == attack->swing_frame) {
                    target_health->take_damage(attack->damage);
                    attack->start_cooldown(); // Use the method from AttackComponent
                    std::cout << ai->type_name << " hits target for " << attack->damage << " damage!" << std::endl;
                }
            }
            return; // Keep attacking if in range and target alive
        }
        
        // Check if auto attack in range
        if (distance <= attack->range) {
            // In range - check BOTH conditions: can attack AND not currently attacking
            if (attack->can_attack()) {
                // Initiate auto attack (both conditions met)
                attack->start_attack();
                
                // DEBUG: Print bottom center positions
                std::cout << "ATTACK DEBUG: " << ai->type_name << " at (" << current_pos.x << ", " << current_pos.y 
                          << ") attacking target at (" << target_pos_cb.x << ", " << target_pos_cb.y << ")" << std::endl;
                
                // Set facing direction toward target when attacking
                Vector2 direction_to_target = {target_pos_cb.x - current_pos.x, target_pos_cb.y - current_pos.y};
                pos->facing_right = (direction_to_target.x > 0);
                std::cout << "  -> " << ai->type_name << " facing_right = " << pos->facing_right << std::endl;
                
                if (anim && anim->attack_anim) {
                    anim->switch_anim(anim->attack_anim.get());
                }
            } else {
                // In range but on cooldown - idle
                if (mov) {
                    mov->move_dx = 0;
                    mov->move_dy = 0;
                }
                if (anim && anim->idle_anim) {
                    anim->switch_anim(anim->idle_anim.get());
                }
            }
        } else {
            // Not in range - move towards enemy with SIMPLE sprite-edge homing
            if (mov) {
                // SIMPLE approach: move to a position next to the target
                float melee_distance = 100.0f; // How close to get for melee
                float ideal_x, ideal_y;
                
                // Determine which side is closer and position accordingly
                if (current_pos.x < target_pos_cb.x) {
                    // Attacker is to the LEFT of target - position to left side
                    ideal_x = target_pos_cb.x - melee_distance;
                    pos->facing_right = true; // Face RIGHT toward target
                    std::cout << "Homing: Position LEFT of target, face RIGHT" << std::endl;
                } else {
                    // Attacker is to the RIGHT of target - position to right side
                    ideal_x = target_pos_cb.x + melee_distance;
                    pos->facing_right = false; // Face LEFT toward target
                    std::cout << "Homing: Position RIGHT of target, face LEFT" << std::endl;
                }
                
                // Same bottom Y level
                ideal_y = target_pos_cb.y;
                
                std::cout << "  Current: (" << current_pos.x << ", " << current_pos.y 
                          << ") Target: (" << target_pos_cb.x << ", " << target_pos_cb.y 
                          << ") Ideal: (" << ideal_x << ", " << ideal_y << ")" << std::endl;
                
                // Move toward ideal position
                float move_dx = ideal_x - current_pos.x;
                float move_dy = ideal_y - current_pos.y;
                float move_distance = sqrt(move_dx*move_dx + move_dy*move_dy);
                
                if (move_distance > mov->speed) {
                    move_dx = (move_dx / move_distance) * mov->speed;
                    move_dy = (move_dy / move_distance) * mov->speed;
                }
                
                mov->move_dx = move_dx;
                mov->move_dy = move_dy;
                
                if (anim && anim->move_anim) {
                    anim->switch_anim(anim->move_anim.get());
                }
            }
        }
    }
    
    void find_closest_target(ECS& ecs, Entity entity, PositionComponent* pos, AIComponent* ai, int target_side) {
        Entity closest_target = -1;
        float closest_distance = FLT_MAX;
        
        auto potential_targets = ecs.get_entities_with<PositionComponent, HealthComponent, AIComponent>();
        
        for (Entity target : potential_targets) {
            if (target == entity) continue;
            
            auto* target_ai = ecs.get_component<AIComponent>(target);
            auto* target_health = ecs.get_component<HealthComponent>(target);
            auto* target_pos = ecs.get_component<PositionComponent>(target);
            
            if (target_ai->side == target_side && !target_health->is_dead) {
                Vector2 current_pos = pos->get_center_bottom();
                Vector2 target_pos_cb = target_pos->get_center_bottom();
                float dx = current_pos.x - target_pos_cb.x;
                float dy = current_pos.y - target_pos_cb.y;
                float center_distance = sqrt(dx*dx + dy*dy);
                float sprite_buffer = 80.0f; // Combined sprite width buffer
                float distance = fmax(0, center_distance - sprite_buffer); // More forgiving range
                
                if (distance < closest_distance) {
                    closest_distance = distance;
                    closest_target = target;
                }
            }
        }
        
        ai->target_entity = closest_target;
        ai->has_target = (closest_target != -1);
    }
};

class HealthSystem {
public:
    void update(ECS& ecs) {
        auto entities = ecs.get_entities_with<HealthComponent>();
        
        for (Entity entity : entities) {
            auto* health = ecs.get_component<HealthComponent>(entity);
            auto* anim = ecs.get_component<AnimationComponent>(entity);
            
            if (health->is_dead) {
                health->remove_timer++;
                if (anim && anim->death_anim) {
                    anim->switch_anim(anim->death_anim.get());
                }
                
                if (health->remove_timer > 3000) {
                    ecs.remove_entity(entity);
                }
            }
        }
    }
    
    void render_health_bars(ECS& ecs) {
        auto entities = ecs.get_entities_with<PositionComponent, HealthComponent>();
        
        for (Entity entity : entities) {
            auto* pos = ecs.get_component<PositionComponent>(entity);
            auto* health = ecs.get_component<HealthComponent>(entity);
            
            if (health->is_dead) continue;
            
            Rectangle healthbar_bg = {pos->x, pos->y - 10, 50, 5};
            Rectangle healthbar_fg = {pos->x, pos->y - 10, 
                                    50 * ((float)health->hp / health->max_hp), 5};
            
            DrawRectangleRec(healthbar_bg, DARKGRAY);
            DrawRectangleRec(healthbar_fg, RED);
        }
    }
};

// ==================== BATTLE SYSTEM ====================

class BattleSystem {
private:
    ECS ecs;
    MovementSystem movement_system;
    AnimationSystem animation_system;
    AttackSystem attack_system;
    HealthSystem health_system;
    
public:
    void initialize() {
        create_test_units();
    }
    
    void update() {
        movement_system.update(ecs);
        attack_system.update(ecs);
        animation_system.update(ecs);
        health_system.update(ecs);
    }
    
    void spawn_skeleton() {
        // Spawn a new skeleton at a random position
        Entity skeleton = ecs.create_entity();
        
        // Random position away from player
        float spawn_x = 300 + (rand() % 200); // Random X between 300-500
        float spawn_y = 400 + (rand() % 200); // Random Y between 400-600
        
        ecs.add_component<PositionComponent>(skeleton, spawn_x, spawn_y, 80, 100);
        ecs.add_component<HealthComponent>(skeleton, 50);
        ecs.add_component<MovementComponent>(skeleton, 0.5f);
        // Proper skeleton attack timing: 8 frames * 10 frame_duration = 80 total, swing at frame 30
        ecs.add_component<AttackComponent>(skeleton, 90, 15, 120, 80, 30);
        ecs.add_component<AIComponent>(skeleton, 1, "Skeleton"); // Enemy side
        
        ecs.add_component<AnimationComponent>(skeleton);
        auto* skeleton_anim = ecs.get_component<AnimationComponent>(skeleton);
        skeleton_anim->set_enemy_offsets(); // Set proper enemy offsets
        skeleton_anim->idle_anim = std::make_unique<Animation>("assets/enemies/skeleton/Idle.png", 4, 150);
        skeleton_anim->move_anim = std::make_unique<Animation>("assets/enemies/skeleton/Walk.png", 4, 150);
        skeleton_anim->attack_anim = std::make_unique<Animation>("assets/enemies/skeleton/Attack.png", 8, 150);
        skeleton_anim->attack_anim->repeat = false;
        skeleton_anim->hit_anim = std::make_unique<Animation>("assets/enemies/skeleton/Take Hit.png", 4, 150);
        skeleton_anim->death_anim = std::make_unique<Animation>("assets/enemies/skeleton/Death.png", 4, 150);
        skeleton_anim->death_anim->repeat = false;
        skeleton_anim->switch_anim(skeleton_anim->idle_anim.get());
        
        std::cout << "Spawned new skeleton at (" << spawn_x << ", " << spawn_y << ")" << std::endl;
    }
    
    void spawn_player(float x = 200, float y = 600) {
        // Spawn a new player knight at specified position
        Entity knight = ecs.create_entity();
        
        ecs.add_component<PositionComponent>(knight, x, y, 80, 100);
        ecs.add_component<HealthComponent>(knight, 1000);
        ecs.add_component<MovementComponent>(knight, 2.0f);
        ecs.add_component<AttackComponent>(knight, 60, 10, 120, 30, 15);
        ecs.add_component<AIComponent>(knight, 0, "Knight"); // Player side
        
        ecs.add_component<AnimationComponent>(knight);
        auto* knight_anim = ecs.get_component<AnimationComponent>(knight);
        knight_anim->set_player_offsets(); // Set proper player offsets
        knight_anim->idle_anim = std::make_unique<Animation>("assets/player/Idle.png", 10);
        knight_anim->move_anim = std::make_unique<Animation>("assets/player/Run.png", 6);
        knight_anim->attack_anim = std::make_unique<Animation>("assets/player/Attack1.png", 4);
        knight_anim->attack_anim->repeat = false;
        knight_anim->hit_anim = std::make_unique<Animation>("assets/player/Get Hit.png", 3);
        knight_anim->death_anim = std::make_unique<Animation>("assets/player/Death.png", 9);
        knight_anim->death_anim->repeat = false;
        knight_anim->switch_anim(knight_anim->idle_anim.get());
        
        std::cout << "Spawned new player knight at (" << x << ", " << y << ")" << std::endl;
    }
    
    void spawn_skeleton_at(float x, float y) {
        // Spawn a new skeleton at specified position
        Entity skeleton = ecs.create_entity();
        
        ecs.add_component<PositionComponent>(skeleton, x, y, 80, 100);
        ecs.add_component<HealthComponent>(skeleton, 50);
        ecs.add_component<MovementComponent>(skeleton, 0.5f);
        // Proper skeleton attack timing: 8 frames * 10 frame_duration = 80 total, swing at frame 30
        ecs.add_component<AttackComponent>(skeleton, 90, 15, 120, 80, 30);
        ecs.add_component<AIComponent>(skeleton, 1, "Skeleton"); // Enemy side
        
        ecs.add_component<AnimationComponent>(skeleton);
        auto* skeleton_anim = ecs.get_component<AnimationComponent>(skeleton);
        skeleton_anim->set_enemy_offsets(); // Set proper enemy offsets
        skeleton_anim->idle_anim = std::make_unique<Animation>("assets/enemies/skeleton/Idle.png", 4, 150);
        skeleton_anim->move_anim = std::make_unique<Animation>("assets/enemies/skeleton/Walk.png", 4, 150);
        skeleton_anim->attack_anim = std::make_unique<Animation>("assets/enemies/skeleton/Attack.png", 8, 150);
        skeleton_anim->attack_anim->repeat = false;
        skeleton_anim->hit_anim = std::make_unique<Animation>("assets/enemies/skeleton/Take Hit.png", 4, 150);
        skeleton_anim->death_anim = std::make_unique<Animation>("assets/enemies/skeleton/Death.png", 4, 150);
        skeleton_anim->death_anim->repeat = false;
        skeleton_anim->switch_anim(skeleton_anim->idle_anim.get());
        
        std::cout << "Spawned new skeleton at (" << x << ", " << y << ")" << std::endl;
    }
    
    void render() {
        animation_system.render(ecs);
        health_system.render_health_bars(ecs);
    }
    
    void create_test_units() {
        // Create player knight
        Entity knight = ecs.create_entity();
        // Use proper hitbox size from backup: 40*scale x 50*scale = 80x100
        ecs.add_component<PositionComponent>(knight, 200, 600, 80, 100);
        ecs.add_component<HealthComponent>(knight, 1000);
        ecs.add_component<MovementComponent>(knight, 2.0f);
        ecs.add_component<AttackComponent>(knight, 60, 10, 120, 30, 15);
        ecs.add_component<AIComponent>(knight, 0, "Knight"); // Player side
        
        ecs.add_component<AnimationComponent>(knight);
        auto* knight_anim = ecs.get_component<AnimationComponent>(knight);
        knight_anim->set_player_offsets(); // Set proper player offsets
        knight_anim->idle_anim = std::make_unique<Animation>("assets/player/Idle.png", 10);
        knight_anim->move_anim = std::make_unique<Animation>("assets/player/Run.png", 6);
        knight_anim->attack_anim = std::make_unique<Animation>("assets/player/Attack1.png", 4);
        knight_anim->attack_anim->repeat = false;
        knight_anim->hit_anim = std::make_unique<Animation>("assets/player/Get Hit.png", 3);
        knight_anim->death_anim = std::make_unique<Animation>("assets/player/Death.png", 9);
        knight_anim->death_anim->repeat = false;
        knight_anim->switch_anim(knight_anim->idle_anim.get());
        
        // Create enemy skeleton with proper attack timing
        Entity skeleton = ecs.create_entity();
        // Use proper hitbox size for skeleton (similar proportions)
        ecs.add_component<PositionComponent>(skeleton, 341, 467, 80, 100);
        ecs.add_component<HealthComponent>(skeleton, 50);
        ecs.add_component<MovementComponent>(skeleton, 0.5f);
        // Skeleton attack: 8 frames * 10 frame_duration = 80 total, swing at frame 3 * 10 = 30
        ecs.add_component<AttackComponent>(skeleton, 90, 15, 120, 80, 30);
        ecs.add_component<AIComponent>(skeleton, 1, "Skeleton"); // Enemy side
        
        ecs.add_component<AnimationComponent>(skeleton);
        auto* skeleton_anim = ecs.get_component<AnimationComponent>(skeleton);
        skeleton_anim->set_enemy_offsets(); // Set proper enemy offsets
        skeleton_anim->idle_anim = std::make_unique<Animation>("assets/enemies/skeleton/Idle.png", 4, 150);
        skeleton_anim->move_anim = std::make_unique<Animation>("assets/enemies/skeleton/Walk.png", 4, 150);
        skeleton_anim->attack_anim = std::make_unique<Animation>("assets/enemies/skeleton/Attack.png", 8, 150);
        skeleton_anim->attack_anim->repeat = false;
        skeleton_anim->hit_anim = std::make_unique<Animation>("assets/enemies/skeleton/Take Hit.png", 4, 150);
        skeleton_anim->death_anim = std::make_unique<Animation>("assets/enemies/skeleton/Death.png", 4, 150);
        skeleton_anim->death_anim->repeat = false;
        skeleton_anim->switch_anim(skeleton_anim->idle_anim.get());
        
        std::cout << "Battle system initialized with ECS architecture" << std::endl;
        std::cout << "Knight entity: " << knight << ", Skeleton entity: " << skeleton << std::endl;
        std::cout << "Press S to spawn a new skeleton for testing" << std::endl;

        // Spawn 2 additional players and 2 additional skeletons
        spawn_player(400, 600);  // Second player
        spawn_player(600, 600);  // Third player
        spawn_skeleton_at(500, 400);  // Second skeleton
        spawn_skeleton_at(700, 400);  // Third skeleton
    }
    
    ECS& get_ecs() { return ecs; }
    
    void handle_input() {
        // Handle spawn command (S key)
        if (IsKeyPressed(KEY_S)) {
            spawn_skeleton();
        }
    }
    
    // Public spawn functions are already implemented above
};

// Global battle system instance
BattleSystem* g_battle_system = nullptr;

// Export functions for use by main.cpp and BattleScene
extern "C" {
    void initialize_battle_system() {
        if (!g_battle_system) {
            g_battle_system = new BattleSystem();
            g_battle_system->initialize();
        }
    }
    
    void update_battle_system() {
        if (g_battle_system) {
            g_battle_system->handle_input(); // Handle spawn commands
            g_battle_system->update();
        }
    }
    
    void render_battle_system() {
        if (g_battle_system) {
            g_battle_system->render();
        }
    }
    
    void cleanup_battle_system() {
        if (g_battle_system) {
            delete g_battle_system;
            g_battle_system = nullptr;
        }
    }
    
    // MOBA control functions
    int get_entity_at_position(float x, float y, int side) {
        if (!g_battle_system) return -1;
        
        auto entities = g_battle_system->get_ecs().get_entities_with<PositionComponent, AIComponent>();
        
        std::cout << "Click at (" << x << ", " << y << ") looking for side " << side << std::endl;
        
        for (Entity entity : entities) {
            auto* pos = g_battle_system->get_ecs().get_component<PositionComponent>(entity);
            auto* ai = g_battle_system->get_ecs().get_component<AIComponent>(entity);
            
            std::cout << "  Entity " << entity << " (" << ai->type_name << ") at (" 
                      << pos->rect.x << ", " << pos->rect.y << ") size " 
                      << pos->rect.width << "x" << pos->rect.height 
                      << " side=" << ai->side << std::endl;
            
            if (ai->side == side) {
                // Check if click is within entity bounds
                if (x >= pos->rect.x && x <= pos->rect.x + pos->rect.width &&
                    y >= pos->rect.y && y <= pos->rect.y + pos->rect.height) {
                    std::cout << "  -> HIT! Selecting entity " << entity << std::endl;
                    return entity;
                }
            }
        }
        std::cout << "  -> No entity found at click position" << std::endl;
        return -1;
    }
    
    void set_entity_target_location(int entity, float x, float y) {
        if (!g_battle_system) return;
        
        auto* ai = g_battle_system->get_ecs().get_component<AIComponent>(entity);
        if (ai) {
            // Create a dummy target entity at the location for movement (like backup system)
            // Clear enemy target and set movement target (like backup system)
            ai->has_target = false;
            ai->target_entity = -1;
            ai->has_move_target = true;
            ai->move_target = {x, y};
            
            std::cout << "Setting move target to (" << x << ", " << y << ")" << std::endl;
        }
    }
    
    void set_entity_target_enemy(int entity, int target_entity) {
        if (!g_battle_system) return;
        
        auto* ai = g_battle_system->get_ecs().get_component<AIComponent>(entity);
        if (ai) {
            ai->target_entity = target_entity;
            ai->has_target = true;
            std::cout << "Player " << ai->type_name << " targeting enemy " << target_entity << std::endl;
        }
    }
    
    // Spawn functions
    void spawn_player_at(float x, float y) {
        if (!g_battle_system) return;
        g_battle_system->spawn_player(x, y);
    }
    
    void spawn_skeleton_at_position(float x, float y) {
        if (!g_battle_system) return;
        g_battle_system->spawn_skeleton_at(x, y);
    }
}