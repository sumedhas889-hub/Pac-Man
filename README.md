# 🕹️ Pac-Man Arcade (C++ & Modern Raylib)

## 📖 Project Description

This project is a recreation of the classic arcade game **Pac-Man**, developed using **Modern C++ (C++17)** and the **Raylib** graphics library. The objective of the project is to build a complete, responsive 2D game engine from scratch, combining fundamental software engineering principles with core game development concepts such as real-time rendering, collision detection, and automated artificial intelligence.

### Technical Highlights & Core Pillars
- **Graphics & Engine Architecture:** Built in modern C++ utilizing Raylib for windowing, user input, and 2D vector rendering. The project follows clean Object-Oriented Programming (OOP) design patterns and uses CMake with `FetchContent` for automated dependency management.
- **Responsive Gameplay & Physics:** Features a tile-based maze system with collision handling, screen-wrapping tunnels, score tracking, animated sprites, and an input buffering system that ensures smooth, responsive cornering.
- **Intelligent Ghost AI & Pathfinding:** Each ghost is programmed with a distinct behavioral personality (chasing, ambushing, flanking, and retreating) along with global mode timers (Scatter, Chase, and Frightened). When ghosts are eaten, pathfinding algorithms (such as BFS) guide their return to the central ghost house.
- **State Management & Game Loop:** Implements a structured finite state machine to manage screen transitions, level progression, player lives, animations, and arcade-style scoring combos.

---

## 🌟 Key Features

- **Authentic Ghost Personalities**:
  - 🔴 **Blinky (Shadow)**: Direct target tracking + Cruise Elroy acceleration.
  - 🌸 **Pinky (Speedy)**: Offset ambush targeting (4 tiles ahead of Pac-Man).
  - 🔷 **Inky (Bashful)**: Dual-vector geometric pincer coordination with Blinky.
  - 🍊 **Clyde (Pokey)**: Proximity-based state switching (Chases when $\ge 8$ tiles, retreats to scatter corner when close).
- **Responsive Steering & Input Buffering**: `std::queue<Direction>` pre-turning system for smooth 90° cornering.
- **Classic Arcade Modes**:
  - **Scatter vs. Chase Waves**: Timed global AI mode intervals.
  - **Frightened Mode**: Energizer consumption turns ghosts blue with flashing warning indicators.
  - **Eaten Eyes Pathfinding**: BFS shortest-path navigation back to the ghost house.
  - **Combo Multipliers**: Consecutive ghost eating awards $200 \rightarrow 400 \rightarrow 800 \rightarrow 1600$ points.
- **Zero-Friction Build**: Uses CMake `FetchContent` to download and compile Raylib automatically without manual installations.

---

## 🎮 Game Controls

| Key | Action |
| :--- | :--- |
| **Arrow Keys / WASD** | Move Pac-Man (supports pre-turn queuing) |
| **Enter / Space** | Restart Game upon Game Over |
| **Escape** | Quit Game |

---

## 🛠️ Building & Running Locally

### Prerequisites
- Any modern C++ compiler supporting **C++17** (MSVC 2019+, GCC 9+, or Clang 10+).
- **CMake** (3.16 or newer).

### Build Instructions (Terminal / PowerShell)

```bash
# 1. Generate build files in a build/ folder
cmake -B build -S .

# 2. Compile the game (Release or Debug)
cmake --build build --config Release

# 3. Launch the game
./build/Release/PacmanGame.exe
# (or ./build/PacmanGame on Linux/MinGW)
```

---

## 📁 Project Architecture

```text
Pac-Man/
├── CMakeLists.txt              # Root CMake configuration (FetchContent Raylib)
├── README.md                   # Project documentation & guide
├── assets/
│   └── maps/
│       └── classic_maze.txt    # 28x36 ASCII maze matrix
├── include/
│   ├── core/
│   │   ├── Constants.hpp       # Dimensions, speeds, timing, enums
│   │   └── Engine.hpp          # Game loop, HUD, state management
│   ├── entities/
│   │   ├── Entity.hpp          # Base entity class (positions, directions)
│   │   ├── Pacman.hpp          # Player physics, input buffer, chomp animation
│   │   ├── Ghost.hpp           # Base ghost class, state transitions, eye rendering
│   │   └── GhostPersonalities.hpp # Blinky, Pinky, Inky, Clyde targeting classes
│   ├── world/
│   │   └── Map.hpp             # 2D Grid loader, tile queries, neon rendering
│   └── ai/
│       └── Pathfinding.hpp     # BFS / A* algorithms and Euclidean intersection logic
└── src/
    ├── main.cpp                # Application entry point
    ├── core/
    │   └── Engine.cpp
    ├── world/
    │   └── Map.cpp
    ├── entities/
    │   ├── Entity.cpp
    │   ├── Pacman.cpp
    │   ├── Ghost.cpp
    │   └── GhostPersonalities.cpp
    └── ai/
        └── Pathfinding.cpp
```
