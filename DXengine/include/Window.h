#ifndef WINDOW_H
#define WINDOW_H

#include "ChiliWin.h"
#include "EngineException.h"
#include "Keyboard.h"

class Window {
 public:
  Window(int width = 800, int height = 600, LPCWSTR windowTitle = L"DXengine");
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window();

  void setTitle(LPCWSTR title) noexcept {
    SetWindowTextW(_hWnd, title);
  };

  // static callback setup messages
  static LRESULT CALLBACK handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) noexcept;
  // static callback handle messages
  static LRESULT CALLBACK handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                         LPARAM lParam) noexcept;

 public:
  Keyboard kbd;

 private:
 //----------------------------------------------------------- WindowClass --//
  class WindowClass {
   public:
    static WindowClass* instance() noexcept;
    static HINSTANCE hInstance() noexcept;
    static LPCWSTR name() noexcept;

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

  class Exception : public EngineException {
   public:
    Exception(const LPCWSTR file, int line, HRESULT hr)
        : EngineException(file, line), _hr(hr) {
      _type = L"Engine Window Exception";
    };

    static std::wstring translateError(HRESULT hr) noexcept;
    virtual LPCWSTR msg() const noexcept override;

    const char* what() const noexcept override {
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
