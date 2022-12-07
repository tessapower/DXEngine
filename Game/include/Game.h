#ifndef GAME_H
#define GAME_H

#include "ChiliWin.h"
#include "Graphics.h"
#include "Window.h"

class Game {
public:
  Game(Window& wnd) : _wnd(wnd), _gfx(wnd) {};

  Game(Game const&) = delete;
  Game& operator=(Game const&) = delete;

  void tick();

private:
  void composeFrame();
  void updateModel();

private:
  Window& _wnd;
  Graphics _gfx;
};

#endif // GAME_H
