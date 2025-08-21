cmake_minimum_required(VERSION 3.11)
project(Searching-game CXX)

# Set C++17 standard (required for modern raylib)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download raylib from source using FetchContent
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)
FetchContent_MakeAvailable(raylib)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

# Create executable with all source files
add_executable(Searching-game ${SOURCES})

# Link raylib to our executable
target_link_libraries(Searching-game raylib)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

# Add a custom target to run the game
add_custom_target(run
    COMMAND Searching-game
    DEPENDS Searching-game
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)