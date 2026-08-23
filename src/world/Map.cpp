#include "world/Map.hpp"
#include "raylib.h"
#include <fstream>
#include <iostream>
#include <cmath>

namespace PacmanGame {

static const std::string DEFAULT_MAZE[GRID_ROWS] = {
    "                            ",
    "                            ",
    "                            ",
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#o####.#####.##.#####.####o#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###--### ##.#     ",
    "######.## #HHHHHH# ##.######",
    "      .   #HHHHHH#   .      ",
    "######.## #HHHHHH# ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "     #.## ######## ##.#     ",
    "######.## ######## ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#o..##................##..o#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################",
    "                            ",
    "                            "
};

Map::Map() {
    loadDefaultMap();
}

void Map::loadDefaultMap() {
    m_initialGrid.resize(GRID_ROWS, std::vector<TileType>(GRID_COLS, TileType::EMPTY));
    m_grid.resize(GRID_ROWS, std::vector<TileType>(GRID_COLS, TileType::EMPTY));

    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            char ch = DEFAULT_MAZE[r][c];
            m_initialGrid[r][c] = static_cast<TileType>(ch);
            m_grid[r][c] = m_initialGrid[r][c];
        }
    }
    countPellets();
}

bool Map::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        loadDefaultMap();
        return false;
    }

    std::string line;
    int r = 0;
    while (std::getline(file, line) && r < GRID_ROWS) {
        for (int c = 0; c < GRID_COLS && c < static_cast<int>(line.length()); ++c) {
            m_initialGrid[r][c] = static_cast<TileType>(line[c]);
            m_grid[r][c] = m_initialGrid[r][c];
        }
        r++;
    }
    countPellets();
    return true;
}

void Map::countPellets() {
    m_remainingPellets = 0;
    m_totalPellets = 0;
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            if (m_grid[r][c] == TileType::PELLET || m_grid[r][c] == TileType::ENERGIZER) {
                m_remainingPellets++;
                m_totalPellets++;
            }
        }
    }
}

void Map::reset() {
    m_grid = m_initialGrid;
    countPellets();
}

TileType Map::getTile(int x, int y) const {
    if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_ROWS) {
        return TileType::EMPTY;
    }
    return m_grid[y][x];
}

void Map::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < GRID_COLS && y >= 0 && y < GRID_ROWS) {
        m_grid[y][x] = type;
    }
}

bool Map::isWall(int x, int y) const {
    return getTile(x, y) == TileType::WALL;
}

bool Map::isGhostHouse(int x, int y) const {
    return getTile(x, y) == TileType::GHOST_HOUSE;
}

bool Map::isGhostGate(int x, int y) const {
    return getTile(x, y) == TileType::GHOST_GATE;
}

bool Map::isIntersection(int x, int y) const {
    int openBranches = 0;
    if (!isWall(x, y - 1)) openBranches++;
    if (!isWall(x, y + 1)) openBranches++;
    if (!isWall(x - 1, y)) openBranches++;
    if (!isWall(x + 1, y)) openBranches++;
    return openBranches >= 3;
}

bool Map::isLegalMove(int x, int y, Direction dir, bool isGhost, bool isGhostEaten) const {
    int targetX = x;
    int targetY = y;
    switch (dir) {
        case Direction::UP:    targetY--; break;
        case Direction::DOWN:  targetY++; break;
        case Direction::LEFT:  targetX--; break;
        case Direction::RIGHT: targetX++; break;
        case Direction::NONE:  return true;
    }

    // Tunnel wrap handles horizontal out of bounds
    if (targetX < 0 || targetX >= GRID_COLS) {
        return true;
    }

    if (targetY < 0 || targetY >= GRID_ROWS) {
        return false;
    }

    TileType tile = getTile(targetX, targetY);
    if (tile == TileType::WALL) {
        return false;
    }

    if (tile == TileType::GHOST_GATE) {
        return isGhost && isGhostEaten; // Eaten ghost or exiting ghost house
    }

    if (tile == TileType::GHOST_HOUSE) {
        return isGhost;
    }

    return true;
}

int Map::eatItem(int x, int y, bool& isEnergizer) {
    isEnergizer = false;
    if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_ROWS) {
        return 0;
    }

    TileType tile = m_grid[y][x];
    if (tile == TileType::PELLET) {
        m_grid[y][x] = TileType::EMPTY;
        m_remainingPellets--;
        return SCORE_PELLET;
    } else if (tile == TileType::ENERGIZER) {
        m_grid[y][x] = TileType::EMPTY;
        m_remainingPellets--;
        isEnergizer = true;
        return SCORE_ENERGIZER;
    }
    return 0;
}

void Map::render(float flashTimer) const {
    Color wallColor = (flashTimer > 0.0f && std::fmod(flashTimer, 0.4f) < 0.2f)
                      ? WHITE
                      : Color{ 33, 33, 222, 255 }; // Classic Arcade Neon Blue
    Color wallFillColor = Color{ 10, 10, 50, 255 };
    Color pelletColor = Color{ 255, 183, 174, 255 }; // Classic Peach Dot
    Color energizerColor = (std::fmod(GetTime() * 4.0, 1.0) < 0.5) 
                           ? Color{ 255, 183, 174, 255 } 
                           : Color{ 255, 100, 100, 255 };
    Color gateColor = Color{ 255, 184, 222, 255 };

    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            TileType tile = m_grid[r][c];
            int px = c * TILE_SIZE;
            int py = r * TILE_SIZE;

            if (tile == TileType::WALL) {
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, wallFillColor);
                DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, wallColor);
            } else if (tile == TileType::PELLET) {
                DrawCircle(px + TILE_SIZE / 2, py + TILE_SIZE / 2, 2.5f, pelletColor);
            } else if (tile == TileType::ENERGIZER) {
                DrawCircle(px + TILE_SIZE / 2, py + TILE_SIZE / 2, 6.0f, energizerColor);
            } else if (tile == TileType::GHOST_GATE) {
                DrawRectangle(px, py + TILE_SIZE / 2 - 2, TILE_SIZE, 4, gateColor);
            }
        }
    }
}

} // namespace PacmanGame
