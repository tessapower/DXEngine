#ifndef WINDOW_H
#define WINDOW_H

#include "ChiliWin.h"

class Window {
 public:
  Window(int width = 800, int height = 600, LPCWSTR name = L"WindowMcWinFace");
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
  class WindowClass {
   public:
    static WindowClass* instance();
    static HINSTANCE hInstance();
    static LPCWSTR name();

   private:
    static WindowClass* _windowClass;
    static constexpr LPCWSTR _name = L"DXengine Window";
    HINSTANCE _hInstance;

   private:
    WindowClass() noexcept;
    ~WindowClass();
    WindowClass(WindowClass& other) = delete;
    void operator=(const WindowClass&) = delete;
  };

 private:
  LRESULT handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

 private:
  int _width;
  int _height;
  HWND _hWnd;
};

#endif  // WINDOW_H
