#ifndef RENDERER_H
#define RENDERER_H

#pragma comment(lib, "d2d1.lib")

#include <d2d1.h>
#include "stdafx.h"
#include "app.h"

class renderer {
public:
  void startFrame();
  void endFrame();
  explicit renderer(const app& app);
  renderer(renderer const&) = delete;
  renderer& operator=(renderer const&) = delete;
  renderer(renderer const&&) = delete;
  renderer& operator=(renderer const&&) = delete;
  ~renderer();


  // clear(color);
  // pixel(x, y, color);
  // rect(x, y, width, height);
  // oval(x, y, width, height);
  // roundedRect ???

private:
  // We use this to recreate the render target and Direct2D resources if an
  // error occurs
  HWND _hWnd;
  RECT _clientRect;

  // Used to create Direct2D resources
  ID2D1Factory* _d2dFactory = nullptr;
  // Where we draw everything
  ID2D1HwndRenderTarget* _renderTarget = nullptr;
  // A brush we can draw things with
  ID2D1SolidColorBrush* _redBrush = nullptr;
  // ???

  /**
   * @brief Safely releases COM interface pointers.
  */
  template <class T> void SafeRelease(T** ppT) {
    if (*ppT) {
      (*ppT)->Release();
      *ppT = nullptr;
    }
  }
};

#endif // RENDERER_H
