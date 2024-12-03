#ifndef APP_H
#define APP_H

#include <d3d11.h>
#include <imgui_impl_win32.h>

#include "stdafx.h"
#include "engine_exception.h"

// Forward declarations of helper functions
auto WINAPI wnd_proc(HWND h_wnd, UINT msg, WPARAM w_param,
                     LPARAM l_param) -> LRESULT;

// Forward declare message handler from imgui_impl_win32.cpp
extern auto ImGui_ImplWin32_WndProcHandler(HWND h_wnd, UINT msg, WPARAM w_param,
                                           LPARAM l_param) -> LRESULT;

class app {
  int width_;
  int height_;
  HWND h_wnd_;

  friend class renderer;

 public:
  explicit app(int width = 800, int height = 600, LPCWSTR window_title = L"");
  app(const app&) = delete;
  app& operator=(const app&) = delete;
  app(const app&&) = delete;
  app& operator=(const app&&) = delete;
  ~app();

  auto show() const noexcept -> void {
    ShowWindow(h_wnd_, SW_SHOWDEFAULT);
    UpdateWindow(h_wnd_);
  }

  void set_title(const LPCWSTR title) const noexcept {
    SetWindowTextW(h_wnd_, title);
  }

  [[nodiscard]] HWND const& h_wnd() const noexcept { return h_wnd_; }

  // static callback setup messages
  static LRESULT CALLBACK handle_msg_setup(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                        LPARAM l_param) noexcept;
  // static callback handle messages
  static LRESULT CALLBACK handle_msg_thunk(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                         LPARAM l_param) noexcept;
private:
  static auto WINAPI handle_msg(HWND h_wnd, UINT u_msg, WPARAM w_param,
                           LPARAM l_param) noexcept -> LRESULT;

public:
  class exception : public engine_exception {
  public:
    exception(const LPCWSTR file, const int line, const HRESULT hr)
        : engine_exception(file, line), hr_(hr) {
      type_ = L"App Exception";
    }

    auto what() const noexcept -> const char* override;

    auto msg() const noexcept -> LPCWSTR override;

    auto error_code() const noexcept -> HRESULT;

    auto error_string() const noexcept -> std::wstring;

    static auto translate_error_code(HRESULT hr) noexcept -> std::wstring;

   private:
   HRESULT hr_;
  };

 private:
  //-------------------------------------------------------- Window Class --//

  // Corresponds to the class of window that is created and can be referenced
  // later by the Win32 API.
  class window_class {
   public:
    static const wchar_t* class_name() noexcept;
    static HINSTANCE h_instance() noexcept;

    window_class(const window_class&) = delete;
    window_class& operator=(const window_class&) = delete;

   private:
    window_class() noexcept;
    ~window_class();
    static constexpr auto name = L"DXEngine Window";
    static window_class wc_;
    HINSTANCE h_instance_;
  };
};

#endif  // APP_H
