#ifndef GAME_H
#define GAME_H

#include "stdafx.h"
#include "renderer.h"
#include "app.h"

class Game {
 public:
  Game(app& wnd) : _wnd(wnd), renderer_(wnd) {}

  Game(Game const&) = delete;
  Game& operator=(Game const&) = delete;

  void tick();

 private:
  void composeFrame();
  void updateModel();

 private:
  app& _wnd;
  renderer renderer_;
};

#endif  // GAME_H
