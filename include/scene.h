#ifndef GAME_H
#define GAME_H
#include "stdafx.h"

#include "renderer/renderer.h"
#include "app.h"

class scene {
 public:
  scene(app& wnd) : _wnd(wnd) {}

  scene(scene const&) = delete;
  scene& operator=(scene const&) = delete;

  void tick();

 private:
  void composeFrame();
  void updateModel();

 private:
  app& _wnd;
};

#endif  // GAME_H
