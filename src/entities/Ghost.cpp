#include "entities/Ghost.hpp"
#include "entities/Pacman.hpp"
#include "ai/Pathfinding.hpp"
#include "raylib.h"
#include <cmath>
#include <cstdlib>

namespace PacmanGame {

Ghost::Ghost(GhostType type, float startX, float startY, GridCoord scatterTile, uint32_t colorHex)
    : Entity(startX, startY, GHOST_SPEED_NORMAL),
      m_type(type),
      m_scatterTile(scatterTile),
      m_homeTile{ static_cast<int>(startX), static_cast<int>(startY) },
      m_colorHex(colorHex) {
    m_state = GhostState::HOUSE;
}

void Ghost::reset() {
    Entity::reset();
    m_state = GhostState::HOUSE;
    m_previousState = GhostState::SCATTER;
    m_frightenedTimer = 0.0f;
    m_houseTimer = 0.0f;
    m_lastIntersection = { -1, -1 };
}

void Ghost::setState(GhostState newState) {
    if (m_state == GhostState::EATEN && newState != GhostState::HOUSE) {
        return; // Eaten ghosts must return to house first
    }

    if (m_state == GhostState::FRIGHTENED && newState == GhostState::CHASE) {
        m_state = GhostState::CHASE;
        m_speed = m_baseSpeed;
    } else if (newState != GhostState::FRIGHTENED) {
        m_state = newState;
    }
}

void Ghost::triggerFrightened() {
    if (m_state == GhostState::EATEN || m_state == GhostState::HOUSE) {
        return;
    }
    m_previousState = (m_state == GhostState::FRIGHTENED) ? m_previousState : m_state;
    m_state = GhostState::FRIGHTENED;
    m_frightenedTimer = FRIGHTENED_DURATION;
    m_speed = GHOST_SPEED_FRIGHTENED;

    // Immediately reverse direction upon becoming frightened
    switch (m_currentDir) {
        case Direction::UP:    m_currentDir = Direction::DOWN; break;
        case Direction::DOWN:  m_currentDir = Direction::UP; break;
        case Direction::LEFT:  m_currentDir = Direction::RIGHT; break;
        case Direction::RIGHT: m_currentDir = Direction::LEFT; break;
        case Direction::NONE:  break;
    }
}

void Ghost::eat() {
    m_state = GhostState::EATEN;
    m_speed = GHOST_SPEED_EATEN;
}

void Ghost::update(float dt, const Map& map) {
    // Overload called with pacman context in updateGhost
}

void Ghost::updateGhost(float dt, const Map& map, const Pacman& pacman, const Ghost* blinky) {
    if (m_state == GhostState::HOUSE) {
        m_houseTimer += dt;
        // Bob up and down inside house
        m_position.y += std::sin(m_houseTimer * 5.0f) * 0.02f;

        float exitDelay = 0.0f;
        switch (m_type) {
            case GhostType::BLINKY: exitDelay = 0.0f; break;
            case GhostType::PINKY:  exitDelay = 2.0f; break;
            case GhostType::INKY:   exitDelay = 5.0f; break;
            case GhostType::CLYDE:  exitDelay = 9.0f; break;
        }

        if (m_houseTimer >= exitDelay) {
            // Exit house gate (Tile 13.5, 14.0)
            m_position = { 13.5f, 14.0f };
            m_currentDir = Direction::LEFT;
            m_state = m_previousState;
            m_speed = m_baseSpeed;
        }
        return;
    }

    if (m_state == GhostState::FRIGHTENED) {
        updateFrightened(dt);
    }

    // Determine target tile based on current ghost state
    GridCoord target = m_scatterTile;
    if (m_state == GhostState::CHASE) {
        target = calculateTargetTile(pacman, blinky);
    } else if (m_state == GhostState::SCATTER) {
        target = m_scatterTile;
    } else if (m_state == GhostState::FRIGHTENED) {
        // Random target
        target = { std::rand() % GRID_COLS, std::rand() % GRID_ROWS };
    } else if (m_state == GhostState::EATEN) {
        target = { 13, 14 }; // Ghost house entry gate
    }

    updateMovement(dt, map, target);

    // If eaten ghost reached house gate, revive
    if (m_state == GhostState::EATEN) {
        GridCoord current = getGridCoord();
        if (current.x == 13 && (current.y == 14 || current.y == 15)) {
            m_state = GhostState::CHASE;
            m_speed = m_baseSpeed;
        }
    }
}

void Ghost::updateFrightened(float dt) {
    m_frightenedTimer -= dt;
    if (m_frightenedTimer <= 0.0f) {
        m_state = m_previousState;
        m_speed = m_baseSpeed;
    }
}

void Ghost::updateMovement(float dt, const Map& map, GridCoord target) {
    GridCoord current = getGridCoord();

    if (isCenteredOnTile() && current != m_lastIntersection) {
        m_lastIntersection = current;
        if (m_state == GhostState::EATEN) {
            m_currentDir = Pathfinding::getNextBfsStep(current, target, map);
        } else {
            m_currentDir = Pathfinding::chooseBestDirection(current, m_currentDir, target, map, false);
        }

        // Snap to center
        if (m_currentDir == Direction::UP || m_currentDir == Direction::DOWN) {
            m_position.x = std::round(m_position.x);
        } else if (m_currentDir == Direction::LEFT || m_currentDir == Direction::RIGHT) {
            m_position.y = std::round(m_position.y);
        }
    }

    if (m_currentDir != Direction::NONE) {
        moveAlongDirection(m_currentDir, m_speed * dt);
    }
}

void Ghost::renderBody(float cx, float cy, uint32_t colorHex) const {
    Color col = Color{
        static_cast<unsigned char>((colorHex >> 24) & 0xFF),
        static_cast<unsigned char>((colorHex >> 16) & 0xFF),
        static_cast<unsigned char>((colorHex >> 8) & 0xFF),
        static_cast<unsigned char>(colorHex & 0xFF)
    };

    float radius = TILE_SIZE * 0.45f;
    // Top dome
    DrawCircle(cx, cy - 2, radius, col);
    // Skirt / body rectangle
    DrawRectangle(cx - radius, cy - 2, radius * 2, radius + 3, col);

    // Animated feet ripples
    float feetAnim = std::sin(GetTime() * 12.0f);
    float footWidth = (radius * 2.0f) / 3.0f;
    for (int i = 0; i < 3; ++i) {
        float fx = cx - radius + i * footWidth + footWidth / 2.0f;
        float fy = cy + radius + 1.0f + (i % 2 == 0 ? feetAnim : -feetAnim);
        DrawCircle(fx, fy, footWidth / 2.5f, col);
    }
}

void Ghost::renderEyes(float cx, float cy) const {
    float eyeOffsetX = 0.0f;
    float eyeOffsetY = 0.0f;
    switch (m_currentDir) {
        case Direction::UP:    eyeOffsetY = -3.0f; break;
        case Direction::DOWN:  eyeOffsetY = 3.0f; break;
        case Direction::LEFT:  eyeOffsetX = -3.0f; break;
        case Direction::RIGHT: eyeOffsetX = 3.0f; break;
        case Direction::NONE:  break;
    }

    // Left eye & pupil
    DrawCircle(cx - 4 + eyeOffsetX, cy - 3 + eyeOffsetY, 3.5f, WHITE);
    DrawCircle(cx - 4 + eyeOffsetX * 1.5f, cy - 3 + eyeOffsetY * 1.5f, 1.8f, BLUE);

    // Right eye & pupil
    DrawCircle(cx + 4 + eyeOffsetX, cy - 3 + eyeOffsetY, 3.5f, WHITE);
    DrawCircle(cx + 4 + eyeOffsetX * 1.5f, cy - 3 + eyeOffsetY * 1.5f, 1.8f, BLUE);
}

void Ghost::render() const {
    float cx = (m_position.x + 0.5f) * TILE_SIZE;
    float cy = (m_position.y + 0.5f) * TILE_SIZE;

    if (m_state == GhostState::EATEN) {
        renderEyes(cx, cy);
        return;
    }

    if (m_state == GhostState::FRIGHTENED) {
        uint32_t frightColor = 0x2121FFFF; // Blue
        if (m_frightenedTimer <= FRIGHTENED_FLASH_TIME && std::fmod(m_frightenedTimer, 0.3f) < 0.15f) {
            frightColor = 0xFFFFFFFF; // White flash
        }
        renderBody(cx, cy, frightColor);

        // Frightened face (small dots)
        DrawCircle(cx - 3, cy - 2, 1.5f, (frightColor == 0xFFFFFFFF) ? RED : ORANGE);
        DrawCircle(cx + 3, cy - 2, 1.5f, (frightColor == 0xFFFFFFFF) ? RED : ORANGE);
        return;
    }

    renderBody(cx, cy, m_colorHex);
    renderEyes(cx, cy);
}

} // namespace PacmanGame
