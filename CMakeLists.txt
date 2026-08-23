cmake_minimum_required(VERSION 3.16)
project(PacmanGame LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib automatically
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Engine.cpp
    src/world/Map.cpp
    src/entities/Entity.cpp
    src/entities/Pacman.cpp
    src/entities/Ghost.cpp
    src/entities/GhostPersonalities.cpp
    src/ai/Pathfinding.cpp
)

# Header include paths
include_directories(include)

# Executable target
add_executable(PacmanGame ${SOURCES})

# Link Raylib
target_link_libraries(PacmanGame PRIVATE raylib)

# Copy assets folder to build directory automatically
add_custom_command(TARGET PacmanGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PacmanGame>/assets
)
