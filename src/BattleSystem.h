#pragma once

extern "C" {
    void initialize_battle_system();
    void update_battle_system();
    void render_battle_system();
    void cleanup_battle_system();
    
    // Functions to interact with ECS for MOBA controls
    int get_entity_at_position(float x, float y, int side); // Returns entity ID or -1
    void set_entity_target_location(int entity, float x, float y);
    void set_entity_target_enemy(int entity, int target_entity);
}