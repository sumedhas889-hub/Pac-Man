#pragma once

#include "core/Constants.hpp"
#include "world/Map.hpp"
#include "entities/Pacman.hpp"
#include "entities/GhostPersonalities.hpp"
#include <memory>
#include <vector>

namespace PacmanGame {

class Engine {
public:
    Engine();
    ~Engine() = default;

    void init();
    void run();

private:
    void handleInput();
    void update(float dt);
    void render();

    void resetLevel(bool fullReset = false);
    void checkCollisions();
    void updateGhostGlobalWaves(float dt);

    Map m_map;
    std::unique_ptr<Pacman> m_pacman;
    std::vector<std::unique_ptr<Ghost>> m_ghosts;
    Blinky* m_blinkyRef = nullptr;

    GameState m_gameState = GameState::READY;
    int m_score = 0;
    int m_highScore = 10000;
    int m_lives = 3;
    int m_level = 1;

    // Frightened / Ghost eating streak
    int m_ghostsEatenStreak = 0;

    // Timers
    float m_stateTimer = 0.0f;
    float m_globalWaveTimer = 0.0f;
    int m_waveIndex = 0;
    bool m_isGlobalScatter = true;

    // Ready / Death animation timers
    float m_deathAnimTimer = 0.0f;
    float m_readyTimer = 0.0f;
    float m_levelClearTimer = 0.0f;
};

} // namespace PacmanGame
