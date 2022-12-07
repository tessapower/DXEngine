#include "Game.h"

void Game::tick() {
  _gfx.startFrame();
  updateModel();
  composeFrame();
  _gfx.endFrame();
}

void Game::composeFrame() {
  // Draw things here
}

void Game::updateModel() {
  // Update things here
}
