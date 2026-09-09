#include "core/Engine.hpp"
#include "raylib.h"
#include <cmath>
#include <string>

namespace PacmanGame {

Engine::Engine() {
    init();
}

void Engine::init() {
    m_pacman = std::make_unique<Pacman>(13.5f, 26.0f);

    auto blinky = std::make_unique<Blinky>(13.5f, 14.0f);
    m_blinkyRef = blinky.get();
    m_ghosts.push_back(std::move(blinky));

    m_ghosts.push_back(std::make_unique<Pinky>(13.5f, 17.0f));
    m_ghosts.push_back(std::make_unique<Inky>(11.5f, 17.0f));
    m_ghosts.push_back(std::make_unique<Clyde>(15.5f, 17.0f));

    resetLevel(true);
}

void Engine::resetLevel(bool fullReset) {
    if (fullReset) {
        m_score = 0;
        m_lives = 3;
        m_level = 1;
        m_map.reset();
    }

    m_pacman->reset();
    for (auto& ghost : m_ghosts) {
        ghost->reset();
    }

    m_gameState = GameState::READY;
    m_readyTimer = 2.0f;
    m_globalWaveTimer = 0.0f;
    m_waveIndex = 0;
    m_isGlobalScatter = true;
    m_ghostsEatenStreak = 0;
}

void Engine::run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pac-Man Arcade (C++ / Modern Raylib)");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f; // Clamp delta time spike

        handleInput();
        update(dt);
        render();
    }

    CloseWindow();
}

void Engine::handleInput() {
    // Pause / Resume
    if (IsKeyPressed(KEY_P) && 
        (m_gameState == GameState::PLAYING || m_gameState == GameState::PAUSED)) {
        
        if (m_gameState == GameState::PLAYING) {
            m_gameState = GameState::PAUSED;
        } else {
            m_gameState = GameState::PLAYING;
        }
    }

    // Restart after Game Over
    if (m_gameState == GameState::GAME_OVER) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            resetLevel(true);
        }
    }
}

void Engine::update(float dt) {
    if (m_score > m_highScore) {
        m_highScore = m_score;
    }

    switch (m_gameState) {
        case GameState::READY:
            m_readyTimer -= dt;
            if (m_readyTimer <= 0.0f) {
                m_gameState = GameState::PLAYING;
            }
            break;

        case GameState::PLAYING: {
            updateGhostGlobalWaves(dt);

            m_pacman->update(dt, m_map);

            // Eat dots
            GridCoord pacCoord = m_pacman->getGridCoord();
            bool isEnergizer = false;
            int points = m_map.eatItem(pacCoord.x, pacCoord.y, isEnergizer);
            if (points > 0) {
                m_score += points;
                if (isEnergizer) {
                    m_ghostsEatenStreak = 0;
                    for (auto& ghost : m_ghosts) {
                        ghost->triggerFrightened();
                    }
                }
            }

            // Check level clear
            if (m_map.getRemainingPellets() == 0) {
                m_gameState = GameState::LEVEL_CLEARED;
                m_levelClearTimer = 3.0f;
                return;
            }

            // Update ghosts
            for (auto& ghost : m_ghosts) {
                ghost->updateGhost(dt, m_map, *m_pacman, m_blinkyRef);
            }

            checkCollisions();
            break;
        }

        case GameState::PACMAN_DYING:
            m_deathAnimTimer += dt;
            if (m_deathAnimTimer >= 1.5f) {
                m_lives--;
                if (m_lives <= 0) {
                    m_gameState = GameState::GAME_OVER;
                } else {
                    resetLevel(false);
                }
            }
            break;

        case GameState::LEVEL_CLEARED:
            m_levelClearTimer -= dt;
            if (m_levelClearTimer <= 0.0f) {
                m_level++;
                m_map.reset();
                resetLevel(false);
            }
            break;

        case GameState::GAME_OVER:
            break;
    }
}

void Engine::updateGhostGlobalWaves(float dt) {
    // Wave timing in seconds: Scatter, Chase, Scatter, Chase...
    const float waveDurations[] = { 7.0f, 20.0f, 7.0f, 20.0f, 5.0f, 20.0f, 5.0f, -1.0f };
    if (m_waveIndex < 7) {
        m_globalWaveTimer += dt;
        if (m_globalWaveTimer >= waveDurations[m_waveIndex]) {
            m_globalWaveTimer = 0.0f;
            m_waveIndex++;
            m_isGlobalScatter = (m_waveIndex % 2 == 0);

            GhostState targetState = m_isGlobalScatter ? GhostState::SCATTER : GhostState::CHASE;
            for (auto& ghost : m_ghosts) {
                ghost->setState(targetState);
            }
        }
    }
}

void Engine::checkCollisions() {
    Position pacPos = m_pacman->getPosition();

    for (auto& ghost : m_ghosts) {
        Position gPos = ghost->getPosition();
        float dist = std::hypot(pacPos.x - gPos.x, pacPos.y - gPos.y);

        if (dist < 0.6f) { // Collision radius overlap
            if (ghost->getState() == GhostState::FRIGHTENED) {
                ghost->eat();
                int multiplierIndex = std::min(m_ghostsEatenStreak, 3);
                int gainedScore = GHOST_SCORES[multiplierIndex];
                m_score += gainedScore;
                m_ghostsEatenStreak++;
            } else if (ghost->getState() == GhostState::CHASE || ghost->getState() == GhostState::SCATTER) {
                m_gameState = GameState::PACMAN_DYING;
                m_deathAnimTimer = 0.0f;
                break;
            }
        }
    }
}

void Engine::render() {
    BeginDrawing();
    ClearBackground(BLACK);

    // 1. Render Map
    float flashTimer = (m_gameState == GameState::LEVEL_CLEARED) ? m_levelClearTimer : 0.0f;
    m_map.render(flashTimer);

    // 2. Render Entities
    if (m_gameState == GameState::PACMAN_DYING) {
        m_pacman->renderDeathAnimation(m_deathAnimTimer / 1.5f);
    } else {
        m_pacman->render();
        for (const auto& ghost : m_ghosts) {
            ghost->render();
        }
    }

    // 3. Render HUD & Overlays
    // Header Scores
    DrawText("1UP", 60, 10, 18, WHITE);
    DrawText(TextFormat("%06d", m_score), 60, 30, 18, WHITE);

    DrawText("HIGH SCORE", 340, 10, 18, WHITE);
    DrawText(TextFormat("%06d", m_highScore), 360, 30, 18, WHITE);

    // Footer Lives icons
    for (int i = 0; i < m_lives - 1; ++i) {
        float lx = 40.0f + i * 26.0f;
        float ly = SCREEN_HEIGHT - 22.0f;
        DrawCircleSector(Vector2{ lx, ly }, 8.0f, 30.0f, 330.0f, 16, YELLOW);
    }

    // State Banner
    if (m_gameState == GameState::READY) {
        DrawText("READY!", SCREEN_WIDTH / 2 - 45, 17 * TILE_SIZE + 4, 22, YELLOW);
    } else if (m_gameState == GameState::GAME_OVER) {
        DrawText("GAME  OVER", SCREEN_WIDTH / 2 - 80, 17 * TILE_SIZE + 4, 24, RED);
        DrawText("Press ENTER or SPACE to Play Again", SCREEN_WIDTH / 2 - 170, 20 * TILE_SIZE, 16, WHITE);
    } else if (m_gameState == GameState::LEVEL_CLEARED) {
        DrawText("STAGE CLEARED!", SCREEN_WIDTH / 2 - 95, 17 * TILE_SIZE + 4, 22, GREEN);
    }

    EndDrawing();
}

} // namespace PacmanGame
