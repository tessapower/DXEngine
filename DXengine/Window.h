#ifndef WINDOW_H
#define WINDOW_H

#include "ChiliWin.h"
#include "EngineException.h"

class Window {
 public:
  Window(int width = 800, int height = 600, LPCSTR windowTitle = "");
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window();

  // static callback setup messages
  static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) noexcept;
  // static callback handle messages
  static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) noexcept;

 private:
  class WindowClass {
   public:
    static WindowClass* instance() noexcept;
    static HINSTANCE hInstance() noexcept;
    static LPCSTR name() noexcept;

   private:
    static WindowClass* _windowClass;
    static constexpr LPCSTR _name = "DXengine Window";
    HINSTANCE _hInstance;

   private:
    WindowClass() noexcept;
    ~WindowClass();
    WindowClass(WindowClass& other) = delete;
    void operator=(const WindowClass&) = delete;
  };

  class Exception : public EngineException {
   public:
    Exception(const LPCSTR file, int line, HRESULT hr)
        : EngineException(file, line), _hr(hr){};

    static std::string translateError(HRESULT hr) noexcept;
    const char *what() const noexcept override;

    virtual const char* type() const noexcept override {
      return "Engine Window Exception";
    }

   private:
    HRESULT _hr;
  };

 private:
  LRESULT handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

 private:
  int _width;
  int _height;
  HWND _hWnd;
};

#endif  // WINDOW_H
