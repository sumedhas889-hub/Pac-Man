#include "entities/Entity.hpp"
#include "world/Map.hpp"
#include <cmath>

namespace PacmanGame {

Entity::Entity(float x, float y, float speed)
    : m_spawnPos{x, y}, m_position{x, y}, m_speed(speed), m_baseSpeed(speed) {
}

void Entity::reset() {
    m_position = m_spawnPos;
    m_currentDir = Direction::NONE;
    m_nextDir = Direction::NONE;
    m_speed = m_baseSpeed;
}

void Entity::setSpawn(float x, float y) {
    m_spawnPos = {x, y};
    m_position = {x, y};
}

GridCoord Entity::getGridCoord() const {
    return {
        static_cast<int>(std::round(m_position.x)),
        static_cast<int>(std::round(m_position.y))
    };
}

bool Entity::isCenteredOnTile() const {
    float diffX = std::abs(m_position.x - std::round(m_position.x));
    float diffY = std::abs(m_position.y - std::round(m_position.y));
    return (diffX < 0.08f && diffY < 0.08f);
}

void Entity::moveAlongDirection(Direction dir, float distance) {
    switch (dir) {
        case Direction::UP:    m_position.y -= distance; break;
        case Direction::DOWN:  m_position.y += distance; break;
        case Direction::LEFT:  m_position.x -= distance; break;
        case Direction::RIGHT: m_position.x += distance; break;
        case Direction::NONE:  break;
    }
    handleTunnelWrapping();
}

void Entity::handleTunnelWrapping() {
    if (m_position.x < -1.0f) {
        m_position.x = static_cast<float>(GRID_COLS);
    } else if (m_position.x > static_cast<float>(GRID_COLS)) {
        m_position.x = -1.0f;
    }
}

} // namespace PacmanGame
