#pragma once

#include "entities/Entity.hpp"

namespace PacmanGame {

class Pacman : public Entity {
public:
    Pacman(float startX, float startY);
    ~Pacman() override = default;

    void handleInput();
    void update(float dt, const Map& map) override;
    void render() const override;
    void renderDeathAnimation(float progress) const;

    void queueDirection(Direction dir) { m_nextDir = dir; }
    void reset() override;

    bool isMoving() const { return m_currentDir != Direction::NONE && !m_isStuck; }

private:
    bool canMoveInDirection(Direction dir, const Map& map) const;
    void updateAnimation(float dt);

    float m_mouthAngle = 45.0f;
    float m_mouthSpeed = 300.0f;
    bool m_mouthOpening = false;
    bool m_isStuck = false;
};

} // namespace PacmanGame
