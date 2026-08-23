#pragma once

#include "entities/Entity.hpp"
#include "world/Map.hpp"

namespace PacmanGame {

class Pacman;

class Ghost : public Entity {
public:
    Ghost(GhostType type, float startX, float startY, GridCoord scatterTile, uint32_t colorHex);
    ~Ghost() override = default;

    void update(float dt, const Map& map) override;
    void render() const override;
    void reset() override;

    // State management
    void setState(GhostState newState);
    GhostState getState() const { return m_state; }
    GhostType getType() const { return m_type; }

    void triggerFrightened();
    void eat();

    // Target calculation (overridden by Blinky, Pinky, Inky, Clyde)
    virtual GridCoord calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const = 0;

    void updateGhost(float dt, const Map& map, const Pacman& pacman, const Ghost* blinky);

protected:
    void updateFrightened(float dt);
    void updateMovement(float dt, const Map& map, GridCoord target);
    void renderEyes(float cx, float cy) const;
    void renderBody(float cx, float cy, uint32_t color) const;

    GhostType m_type;
    GhostState m_state = GhostState::HOUSE;
    GhostState m_previousState = GhostState::SCATTER;

    GridCoord m_scatterTile;
    GridCoord m_homeTile;
    uint32_t m_colorHex;

    float m_frightenedTimer = 0.0f;
    float m_houseTimer = 0.0f;
    float m_houseExitDelay = 0.0f;

    GridCoord m_lastIntersection = { -1, -1 };
};

} // namespace PacmanGame
