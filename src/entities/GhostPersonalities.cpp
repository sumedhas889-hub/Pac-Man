#include "entities/GhostPersonalities.hpp"
#include "entities/Pacman.hpp"
#include "ai/Pathfinding.hpp"
#include <cmath>

namespace PacmanGame {

// BLINKY (Red / Shadow)
Blinky::Blinky(float startX, float startY)
    : Ghost(GhostType::BLINKY, startX, startY, { 25, 0 }, 0xFD0000FF) {
}

GridCoord Blinky::calculateTargetTile(const Pacman& pacman, const Ghost* /*blinky*/) const {
    return pacman.getGridCoord();
}

// PINKY (Pink / Speedy)
Pinky::Pinky(float startX, float startY)
    : Ghost(GhostType::PINKY, startX, startY, { 2, 0 }, 0xFEA8DFFF) {
}

GridCoord Pinky::calculateTargetTile(const Pacman& pacman, const Ghost* /*blinky*/) const {
    GridCoord pacCoord = pacman.getGridCoord();
    Direction dir = pacman.getDirection();
    int targetX = pacCoord.x;
    int targetY = pacCoord.y;

    switch (dir) {
        case Direction::UP:
            targetY -= 4;
            targetX -= 4; // Classic arcade overflow recreation
            break;
        case Direction::DOWN:
            targetY += 4;
            break;
        case Direction::LEFT:
            targetX -= 4;
            break;
        case Direction::RIGHT:
        case Direction::NONE:
            targetX += 4;
            break;
    }
    return { targetX, targetY };
}

// INKY (Cyan / Bashful)
Inky::Inky(float startX, float startY)
    : Ghost(GhostType::INKY, startX, startY, { 27, 35 }, 0x00FFFFFF) {
}

GridCoord Inky::calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const {
    GridCoord pacCoord = pacman.getGridCoord();
    Direction dir = pacman.getDirection();

    // Intermediate tile 2 ahead of Pacman
    int intermediateX = pacCoord.x;
    int intermediateY = pacCoord.y;

    switch (dir) {
        case Direction::UP:    intermediateY -= 2; break;
        case Direction::DOWN:  intermediateY += 2; break;
        case Direction::LEFT:  intermediateX -= 2; break;
        case Direction::RIGHT:
        case Direction::NONE:  intermediateX += 2; break;
    }

    GridCoord blinkyCoord = blinky ? blinky->getGridCoord() : GridCoord{ 13, 11 };
    // Double the vector from Blinky to Intermediate point
    int targetX = intermediateX + (intermediateX - blinkyCoord.x);
    int targetY = intermediateY + (intermediateY - blinkyCoord.y);

    return { targetX, targetY };
}

// CLYDE (Orange / Pokey)
Clyde::Clyde(float startX, float startY)
    : Ghost(GhostType::CLYDE, startX, startY, { 0, 35 }, 0xFFB852FF) {
}

GridCoord Clyde::calculateTargetTile(const Pacman& pacman, const Ghost* /*blinky*/) const {
    GridCoord clydeCoord = getGridCoord();
    GridCoord pacCoord = pacman.getGridCoord();

    int distSq = Pathfinding::distanceSquared(clydeCoord, pacCoord);
    // If distance >= 8 tiles (64 squared), target Pac-Man; otherwise retreat to scatter corner
    if (distSq >= 64) {
        return pacCoord;
    }
    return m_scatterTile;
}

} // namespace PacmanGame
