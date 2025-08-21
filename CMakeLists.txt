cmake_minimum_required(VERSION 3.10)
project(Searching-game)

# Add the lib directory to include paths
include_directories(lib)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Create executable with all source files
add_executable(Searching-game ${SOURCES})

# Link against raylib library
target_link_libraries(Searching-game ${CMAKE_SOURCE_DIR}/lib/libraylib.a)

# For macOS, we need to link against additional frameworks
if(APPLE)
    target_link_libraries(Searching-game 
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework CoreVideo"
        "-framework CoreAudio"
        "-framework AudioToolbox"
    )
endif()

# Add a custom target to run the game
add_custom_target(run
    COMMAND Searching-game
    DEPENDS Searching-game
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)