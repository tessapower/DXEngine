#ifndef WINDOW_H
#define WINDOW_H

#include "ChiliWin.h"

class Window {
 public:
  explicit Window(int width = 800, int height = 600, LPCWSTR windowTitle = L"");
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(const Window&&) = delete;
  Window& operator=(const Window&&) = delete;
  ~Window();

  void setTitle(LPCWSTR title) const noexcept {
    SetWindowTextW(_hWnd, title);
  }

  // static callback setup messages
  static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) noexcept;
  // static callback handle messages
  static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) noexcept;

 private:
 //----------------------------------------------------------- WindowClass --//
  class WindowClass {
   public:
    static WindowClass* instance() noexcept;
    static HINSTANCE hInstance() noexcept;
    static LPCWSTR name() noexcept;

   private:
    static WindowClass* _windowClass;
    static constexpr LPCWSTR _name = L"DXEngine Window";
    HINSTANCE _hInstance;

   private:
    WindowClass() noexcept;
    ~WindowClass();
  public:
    WindowClass(WindowClass& other) = delete;
    void operator=(const WindowClass&) = delete;
    WindowClass(WindowClass&& other) = delete;
    void operator=(const WindowClass&&) = delete;
  };

  LRESULT handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

  friend class Graphics;

  HWND const& hWnd() const noexcept {
    return _hWnd;
  }

  int _width;
  int _height;
  HWND _hWnd;
};

#endif  // WINDOW_H
