#pragma once

#include <cstdint>

namespace PacmanGame {

    // Grid Dimensions (Original Arcade standard: 28 columns x 36 rows)
    constexpr int GRID_COLS = 28;
    constexpr int GRID_ROWS = 36;

    // Rendering Scale
    constexpr int TILE_SIZE = 20; // Pixels per tile
    constexpr int SCREEN_WIDTH = GRID_COLS * TILE_SIZE;  // 560 px
    constexpr int SCREEN_HEIGHT = GRID_ROWS * TILE_SIZE; // 720 px

    // Game Speeds (Tiles per second)
    constexpr float PACMAN_SPEED = 7.5f;
    constexpr float GHOST_SPEED_NORMAL = 7.0f;
    constexpr float GHOST_SPEED_FRIGHTENED = 4.0f;
    constexpr float GHOST_SPEED_EATEN = 14.0f;

    // Timing Constants (Seconds)
    constexpr float FRIGHTENED_DURATION = 7.0f;
    constexpr float FRIGHTENED_FLASH_TIME = 2.0f; // Flashes white during last 2 seconds
    
    // Scoring
    constexpr int SCORE_PELLET = 10;
    constexpr int SCORE_ENERGIZER = 50;
    constexpr int GHOST_SCORES[] = { 200, 400, 800, 1600 };

    // Directions
    enum class Direction : int8_t {
        NONE = 0,
        UP,
        LEFT,
        DOWN,
        RIGHT
    };

    // Tile Types in the Maze Matrix
    enum class TileType : char {
        EMPTY = ' ',
        WALL = '#',
        PELLET = '.',
        ENERGIZER = 'o',
        GHOST_GATE = '-',
        GHOST_HOUSE = 'H'
    };

    // Ghost Personalities
    enum class GhostType {
        BLINKY, // Red - Shadow
        PINKY,  // Pink - Speedy
        INKY,   // Cyan - Bashful
        CLYDE   // Orange - Pokey
    };

    // Ghost States
    enum class GhostState {
        HOUSE,
        SCATTER,
        CHASE,
        FRIGHTENED,
        EATEN
    };

    // Game State
    enum class GameState {
        READY,
        PLAYING,
        PACMAN_DYING,
        GAME_OVER,
        LEVEL_CLEARED
    };

    struct Position {
        float x = 0.0f;
        float y = 0.0f;

        bool operator==(const Position& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct GridCoord {
        int x = 0;
        int y = 0;

        bool operator==(const GridCoord& other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const GridCoord& other) const {
            return !(*this == other);
        }

        bool operator<(const GridCoord& other) const {
            if (y != other.y) return y < other.y;
            return x < other.x;
        }
    };

} // namespace PacmanGame
