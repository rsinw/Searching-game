Convert C from raylib to C++
(Note: cannot use C macros like raylib colors ex. RAYWHITE)

Initialize window with fixed screenwidth and screenheight, window title named "Searching"

Set FPS to 60. In the main game loop, aims to only move to the next frame 1/60 seconds after the frame started 

---

First screen, menu 

PLAY button
SAVE button
LOAD button


What are we saving?

temp_data should be wiped and recreated on startup

Example of data structure:

temp_data
    inv_items.txt
    gen_stats.txt
        FLOOR="0"
        x_pos="0"
        y_pos="0"
    clear_rooms.txt
        00
    

    char_1
        stats.txt
            TYPE="KNIGHT"
            MAXHP="100"
            HP="100"
            DEF="1"
            ATK="10"
            LVL="1"
            EXP_TO_NEXT_LVL="100"

        items.txt
            HEAD="NONE"
            BODY="NONE"
            WEAPON="NONE"
            TRINKET1="NONE"
            TRINKET2="NONE"






