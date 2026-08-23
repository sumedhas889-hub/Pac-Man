#include "ai/Pathfinding.hpp"
#include <queue>
#include <map>
#include <algorithm>
#include <climits>

namespace PacmanGame {

int Pathfinding::distanceSquared(GridCoord a, GridCoord b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

Direction Pathfinding::chooseBestDirection(
    GridCoord current,
    Direction currentDir,
    GridCoord target,
    const Map& map,
    bool isEaten
) {
    // Reverse directions mapping
    auto isOpposite = [](Direction d1, Direction d2) {
        if (d1 == Direction::UP && d2 == Direction::DOWN) return true;
        if (d1 == Direction::DOWN && d2 == Direction::UP) return true;
        if (d1 == Direction::LEFT && d2 == Direction::RIGHT) return true;
        if (d1 == Direction::RIGHT && d2 == Direction::LEFT) return true;
        return false;
    };

    // Arcade priority order: UP > LEFT > DOWN > RIGHT
    const Direction candidates[] = {
        Direction::UP,
        Direction::LEFT,
        Direction::DOWN,
        Direction::RIGHT
    };

    Direction bestDir = Direction::NONE;
    int minDistance = INT_MAX;

    for (Direction dir : candidates) {
        // Disallow 180-degree reverse
        if (currentDir != Direction::NONE && isOpposite(dir, currentDir)) {
            continue;
        }

        int nextX = current.x;
        int nextY = current.y;
        switch (dir) {
            case Direction::UP:    nextY--; break;
            case Direction::DOWN:  nextY++; break;
            case Direction::LEFT:  nextX--; break;
            case Direction::RIGHT: nextX++; break;
            case Direction::NONE:  break;
        }

        if (!map.isLegalMove(current.x, current.y, dir, true, isEaten)) {
            continue;
        }

        int dist = distanceSquared({ nextX, nextY }, target);
        if (dist < minDistance) {
            minDistance = dist;
            bestDir = dir;
        }
    }

    // Fallback if trapped
    if (bestDir == Direction::NONE) {
        for (Direction dir : candidates) {
            if (map.isLegalMove(current.x, current.y, dir, true, isEaten)) {
                return dir;
            }
        }
    }

    return bestDir;
}

Direction Pathfinding::getNextBfsStep(GridCoord current, GridCoord target, const Map& map) {
    if (current == target) {
        return Direction::NONE;
    }

    std::queue<GridCoord> q;
    std::map<GridCoord, GridCoord> parent;
    std::map<GridCoord, Direction> firstMove;

    q.push(current);

    const Direction dirs[] = { Direction::UP, Direction::LEFT, Direction::DOWN, Direction::RIGHT };

    while (!q.empty()) {
        GridCoord u = q.front();
        q.pop();

        if (u == target) {
            return firstMove[u];
        }

        for (Direction dir : dirs) {
            int nx = u.x;
            int ny = u.y;
            switch (dir) {
                case Direction::UP:    ny--; break;
                case Direction::DOWN:  ny++; break;
                case Direction::LEFT:  nx--; break;
                case Direction::RIGHT: nx++; break;
                case Direction::NONE:  break;
            }

            GridCoord nextCoord = { nx, ny };
            if (!map.isLegalMove(u.x, u.y, dir, true, true)) {
                continue;
            }

            if (parent.find(nextCoord) == parent.end() && nextCoord != current) {
                parent[nextCoord] = u;
                if (u == current) {
                    firstMove[nextCoord] = dir;
                } else {
                    firstMove[nextCoord] = firstMove[u];
                }
                q.push(nextCoord);
            }
        }
    }

    return chooseBestDirection(current, Direction::NONE, target, map, true);
}

} // namespace PacmanGame
