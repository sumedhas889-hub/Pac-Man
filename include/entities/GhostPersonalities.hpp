#pragma once

#include "entities/Ghost.hpp"

namespace PacmanGame {

class Blinky : public Ghost {
public:
    Blinky(float startX, float startY);
    ~Blinky() override = default;

    GridCoord calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const override;
};

class Pinky : public Ghost {
public:
    Pinky(float startX, float startY);
    ~Pinky() override = default;

    GridCoord calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const override;
};

class Inky : public Ghost {
public:
    Inky(float startX, float startY);
    ~Inky() override = default;

    GridCoord calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const override;
};

class Clyde : public Ghost {
public:
    Clyde(float startX, float startY);
    ~Clyde() override = default;

    GridCoord calculateTargetTile(const Pacman& pacman, const Ghost* blinky) const override;
};

} // namespace PacmanGame
