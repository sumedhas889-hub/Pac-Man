#pragma once

#include "core/Constants.hpp"

namespace PacmanGame {

class Map;

class Entity {
public:
    Entity(float x, float y, float speed);
    virtual ~Entity() = default;

    virtual void update(float dt, const Map& map) = 0;
    virtual void render() const = 0;
    virtual void reset();

    Position getPosition() const { return m_position; }
    GridCoord getGridCoord() const;
    Direction getDirection() const { return m_currentDir; }
    void setDirection(Direction dir) { m_currentDir = dir; }

    void setSpawn(float x, float y);
    bool isCenteredOnTile() const;

protected:
    void moveAlongDirection(Direction dir, float distance);
    void handleTunnelWrapping();

    Position m_spawnPos;
    Position m_position;
    Direction m_currentDir = Direction::NONE;
    Direction m_nextDir = Direction::NONE;
    float m_speed;
    float m_baseSpeed;
};

} // namespace PacmanGame
