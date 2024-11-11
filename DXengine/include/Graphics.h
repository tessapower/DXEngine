#ifndef GRAPHICS_H
#define GRAPHICS_H

#pragma comment(lib, "d2d1.lib")

#include <d2d1.h>
#include "Window.h"
#include "stdafx.h"

class Graphics {
public:
  Graphics(Window& wnd);
  Graphics(Graphics const&) = delete;
  Graphics& operator=(Graphics const&) = delete;
  ~Graphics();

  void startFrame();
  void endFrame();


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

#endif // GRAPHICS_H
