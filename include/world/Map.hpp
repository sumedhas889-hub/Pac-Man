#pragma once

#include "core/Constants.hpp"
#include <vector>
#include <string>

namespace PacmanGame {

class Map {
public:
    Map();
    ~Map() = default;

    bool loadFromFile(const std::string& filepath);
    void reset();

    TileType getTile(int x, int y) const;
    void setTile(int x, int y, TileType type);

    bool isWall(int x, int y) const;
    bool isGhostHouse(int x, int y) const;
    bool isGhostGate(int x, int y) const;
    bool isIntersection(int x, int y) const;
    bool isLegalMove(int x, int y, Direction dir, bool isGhost, bool isGhostEaten) const;

    // Eating dots
    int eatItem(int x, int y, bool& isEnergizer);
    int getRemainingPellets() const { return m_remainingPellets; }
    int getTotalPellets() const { return m_totalPellets; }

    void render(float flashTimer = 0.0f) const;

private:
    void loadDefaultMap();
    void countPellets();

    std::vector<std::vector<TileType>> m_initialGrid;
    std::vector<std::vector<TileType>> m_grid;
    int m_remainingPellets = 0;
    int m_totalPellets = 0;
};

} // namespace PacmanGame
