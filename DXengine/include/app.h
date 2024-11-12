#ifndef APP_H
#define APP_H

#include "stdafx.h"

class app {
 public:
  explicit app(int width = 800, int height = 600, LPCWSTR window_title = L"");
  app(const app&) = delete;
  app& operator=(const app&) = delete;
  app(const app&&) = delete;
  app& operator=(const app&&) = delete;
  ~app();

  void set_title(const LPCWSTR title) const noexcept {
    SetWindowTextW(h_wnd_, title);
  }

  // static callback setup messages
  static LRESULT CALLBACK handle_msg_setup(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                        LPARAM l_param) noexcept;
  // static callback handle messages
  static LRESULT CALLBACK handle_msg_thunk(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                         LPARAM l_param) noexcept;

 private:
 //----------------------------------------------------------- WindowClass --//
  class window_class {
   public:
    static window_class* instance() noexcept;
    static HINSTANCE h_instance() noexcept;
    static LPCWSTR class_name() noexcept;

   private:
    static window_class* window_class_;
    static constexpr LPCWSTR name = L"DXEngine Window";
    HINSTANCE h_instance_;

   private:
    window_class() noexcept;
    ~window_class();
  public:
    window_class(window_class& other) = delete;
    void operator=(const window_class&) = delete;
    window_class(window_class&& other) = delete;
    void operator=(const window_class&&) = delete;
  };

  static LRESULT handle_msg(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) noexcept;

  friend class Graphics;

  [[nodiscard]] HWND const& h_wnd() const noexcept { return h_wnd_; }

  int width_;
  int height_;
  HWND h_wnd_;
};

#endif  // APP_H
