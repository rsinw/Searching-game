cmake_minimum_required(VERSION 3.10)
project(SearchingGame)

# Add the lib directory to include paths
include_directories(lib)

# Create executable (using .cpp file)
add_executable(SearchingGame main.cpp)

# Link against raylib library
target_link_libraries(SearchingGame ${CMAKE_SOURCE_DIR}/lib/libraylib.a)

# For macOS, we need to link against additional frameworks
if(APPLE)
    target_link_libraries(SearchingGame 
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
    COMMAND SearchingGame
    DEPENDS SearchingGame
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)