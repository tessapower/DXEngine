#include "stdafx.h"
#include "Game.h"

void Game::tick() {
  renderer_.start_frame();
  updateModel();
  composeFrame();
  renderer_.end_frame();
}

void Game::composeFrame() {
  // Draw things here
}

void Game::updateModel() {
  // Update things here
}
