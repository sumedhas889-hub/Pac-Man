@echo off
git add .
git commit -m "docs: update project description and technical pillars in README"
(goto) 2>nul & del "%~f0"
