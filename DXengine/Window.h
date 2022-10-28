#ifndef WINDOW_H
#define WINDOW_H

#include "ChiliWin.h"

class Window {
 public:
  Window(int width = 800, int height = 600, LPCWSTR name = L"WindowsMcWinFace");
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window();

  // static callback setup messages
  static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam);
  // static callback handle messages
  static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam);

 private:
  LRESULT handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  HINSTANCE hInstance;
  int width;
  int height;
  HWND hWnd;
};

#endif  // WINDOW_H
