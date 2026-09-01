@echo off
git add .
git commit -m "feat: complete C++ Pac-Man game implementation with Raylib, Ghost AI, and Map engine"
(goto) 2>nul & del "%~f0"
