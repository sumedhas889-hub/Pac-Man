#pragma once

#include "core/Constants.hpp"
#include "world/Map.hpp"
#include <vector>

namespace PacmanGame {

class Pathfinding {
public:
    // Calculates squared Euclidean distance between two grid points
    static int distanceSquared(GridCoord a, GridCoord b);

    // Determines best direction at an intersection based on minimum distance to target tile
    // Adheres strictly to the arcade rules: No 180 reverses, and tie-breaking: UP > LEFT > DOWN > RIGHT
    static Direction chooseBestDirection(
        GridCoord current,
        Direction currentDir,
        GridCoord target,
        const Map& map,
        bool isEaten = false
    );

    // BFS shortest path to return eaten ghost eyes back to the ghost house
    static Direction getNextBfsStep(GridCoord current, GridCoord target, const Map& map);
};

} // namespace PacmanGame
