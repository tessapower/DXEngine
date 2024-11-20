#ifndef RENDERER_H
#define RENDERER_H

#pragma comment(lib, "d2d1.lib")

#include <d2d1.h>
#include "stdafx.h"
#include "app.h"

class renderer {
public:
  explicit renderer(const app& app);
  renderer(renderer const&) = delete;
  renderer& operator=(renderer const&) = delete;
  renderer(renderer const&&) = delete;
  renderer& operator=(renderer const&&) = delete;
  ~renderer();

  auto start_frame() const -> void;
  auto end_frame() const -> void;

  // clear(color);
  // pixel(x, y, color);
  // rect(x, y, width, height);
  // oval(x, y, width, height);
  // roundedRect ???

private:
  // We use this to recreate the render target and Direct2D resources if an
  // error occurs
  HWND h_wnd_;
  RECT client_rect_;

  // Used to create Direct2D resources
  ID2D1Factory* d2d_factory_ = nullptr;
  // Where we draw everything
  ID2D1HwndRenderTarget* render_target_ = nullptr;
  // A brush we can draw things with
  ID2D1SolidColorBrush* red_brush_ = nullptr;

  /**
   * @brief Safely releases COM interface pointers.
  */
  template <class T>
  static void safe_release(T** pp_t) {
    if (*pp_t) {
      (*pp_t)->Release();
      *pp_t = nullptr;
    }
  }
};

#endif // RENDERER_H
