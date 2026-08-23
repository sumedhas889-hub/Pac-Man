#include "entities/Pacman.hpp"
#include "world/Map.hpp"
#include "raylib.h"
#include <cmath>

namespace PacmanGame {

Pacman::Pacman(float startX, float startY)
    : Entity(startX, startY, PACMAN_SPEED) {
}

void Pacman::reset() {
    Entity::reset();
    m_mouthAngle = 45.0f;
    m_mouthOpening = false;
    m_isStuck = false;
}

void Pacman::handleInput() {
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        m_nextDir = Direction::UP;
    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        m_nextDir = Direction::DOWN;
    } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        m_nextDir = Direction::LEFT;
    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        m_nextDir = Direction::RIGHT;
    }
}

bool Pacman::canMoveInDirection(Direction dir, const Map& map) const {
    if (dir == Direction::NONE) return false;

    GridCoord current = getGridCoord();
    int nextX = current.x;
    int nextY = current.y;

    switch (dir) {
        case Direction::UP:    nextY--; break;
        case Direction::DOWN:  nextY++; break;
        case Direction::LEFT:  nextX--; break;
        case Direction::RIGHT: nextX++; break;
        case Direction::NONE:  break;
    }

    return !map.isWall(nextX, nextY) && !map.isGhostGate(nextX, nextY);
}

void Pacman::update(float dt, const Map& map) {
    handleInput();

    // 1. Check if we can execute a 180-degree immediate reversal
    if ((m_nextDir == Direction::UP && m_currentDir == Direction::DOWN) ||
        (m_nextDir == Direction::DOWN && m_currentDir == Direction::UP) ||
        (m_nextDir == Direction::LEFT && m_currentDir == Direction::RIGHT) ||
        (m_nextDir == Direction::RIGHT && m_currentDir == Direction::LEFT)) {
        m_currentDir = m_nextDir;
    }

    // 2. Check if queued turn can be made when close to tile center
    if (m_nextDir != m_currentDir && isCenteredOnTile()) {
        if (canMoveInDirection(m_nextDir, map)) {
            m_currentDir = m_nextDir;
            // Snap perpendicular axis to tile center
            if (m_currentDir == Direction::UP || m_currentDir == Direction::DOWN) {
                m_position.x = std::round(m_position.x);
            } else if (m_currentDir == Direction::LEFT || m_currentDir == Direction::RIGHT) {
                m_position.y = std::round(m_position.y);
            }
        }
    }

    // 3. Move in current direction if path ahead is not a wall
    if (m_currentDir != Direction::NONE) {
        if (canMoveInDirection(m_currentDir, map) || !isCenteredOnTile()) {
            moveAlongDirection(m_currentDir, m_speed * dt);
            m_isStuck = false;
            updateAnimation(dt);
        } else {
            // Stop at center of wall tile
            m_position.x = std::round(m_position.x);
            m_position.y = std::round(m_position.y);
            m_isStuck = true;
        }
    }
}

void Pacman::updateAnimation(float dt) {
    if (m_mouthOpening) {
        m_mouthAngle += m_mouthSpeed * dt;
        if (m_mouthAngle >= 45.0f) {
            m_mouthAngle = 45.0f;
            m_mouthOpening = false;
        }
    } else {
        m_mouthAngle -= m_mouthSpeed * dt;
        if (m_mouthAngle <= 2.0f) {
            m_mouthAngle = 2.0f;
            m_mouthOpening = true;
        }
    }
}

void Pacman::render() const {
    float cx = (m_position.x + 0.5f) * TILE_SIZE;
    float cy = (m_position.y + 0.5f) * TILE_SIZE;
    float radius = TILE_SIZE * 0.48f;

    float baseAngle = 0.0f;
    switch (m_currentDir) {
        case Direction::RIGHT: baseAngle = 0.0f; break;
        case Direction::DOWN:  baseAngle = 90.0f; break;
        case Direction::LEFT:  baseAngle = 180.0f; break;
        case Direction::UP:    baseAngle = 270.0f; break;
        case Direction::NONE:  baseAngle = 0.0f; break;
    }

    float startAngle = baseAngle + m_mouthAngle;
    float endAngle = baseAngle + 360.0f - m_mouthAngle;

    Color pacmanYellow = Color{ 255, 255, 0, 255 };
    DrawCircleSector(Vector2{ cx, cy }, radius, startAngle, endAngle, 32, pacmanYellow);
}

void Pacman::renderDeathAnimation(float progress) const {
    float cx = (m_position.x + 0.5f) * TILE_SIZE;
    float cy = (m_position.y + 0.5f) * TILE_SIZE;
    float radius = TILE_SIZE * 0.48f;

    // Progress 0.0 -> 1.0 : mouth opens to 180 deg (full disappearance)
    float angle = progress * 180.0f;
    if (angle < 180.0f) {
        DrawCircleSector(Vector2{ cx, cy }, radius, 270.0f + angle, 270.0f + 360.0f - angle, 32, YELLOW);
    }
}

} // namespace PacmanGame
