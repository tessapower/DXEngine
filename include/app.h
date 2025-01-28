#ifndef APP_H
#define APP_H
#include "stdafx.h"

#include <d3d11.h>

#include <imgui.h>
#include <memory>

#include "renderer/drawable/cube.h"
#include "engine_exception.h"
#include "renderer/renderer.h"
#include "gui.h"
#include "step_timer.h"

// Forward declarations of helper functions
auto WINAPI wnd_proc(HWND h_wnd, UINT msg, WPARAM w_param,
                     LPARAM l_param) -> LRESULT;

// Forward declare message handler from imgui_impl_win32.cpp
extern auto ImGui_ImplWin32_WndProcHandler(HWND h_wnd, UINT msg, WPARAM w_param,
                                           LPARAM l_param) -> LRESULT;

class app {
  float width_;
  float height_;
  HWND h_wnd_;
  std::unique_ptr<renderer> p_renderer_ = nullptr;
  const float clear_color_[4] = {0.45f, 0.55f, 0.60f, 1.00f};
  StepTimer step_timer_;

 public:
  explicit app(float width = 800.0f, float height = 600.0f, LPCSTR window_title = "");
  app(const app&) = delete;
  app& operator=(const app&) = delete;
  app(const app&&) = delete;
  app& operator=(const app&&) = delete;
  ~app();

  auto init_gui() noexcept -> void;

  auto set_title(const LPCSTR title) const noexcept -> void;

  auto tick(bool& done) -> void;

  [[nodiscard]] HWND const& h_wnd() const noexcept { return h_wnd_; }

  // static callback setup messages
  static auto CALLBACK handle_msg_setup(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                        LPARAM l_param) noexcept -> LRESULT;
  // static callback handle messages
  static auto CALLBACK handle_msg_thunk(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                        LPARAM l_param) noexcept -> LRESULT;

 private:
  auto process_messages(bool& done) -> void;

  auto update(const StepTimer& timer) noexcept -> void;

  auto render() noexcept -> void;

  auto WINAPI handle_msg(HWND h_wnd, UINT u_msg, WPARAM w_param,
                                LPARAM l_param) noexcept -> LRESULT;

 public:
  class exception final : public hr_exception {
   public:
    exception(const LPCSTR file, const int line, const HRESULT hr)
        : hr_exception(file, line, hr) {
      type_ = "App Exception";
    }
  };

 private:
  //-------------------------------------------------------- Window Class --//

  // Corresponds to the class of window that is created and can be referenced
  // later by the Win32 API.
  class window_class {
   public:
    static auto class_name() noexcept -> const char*;
    static auto h_instance() noexcept -> HINSTANCE;

    window_class(const window_class&) = delete;
    window_class& operator=(const window_class&) = delete;
    window_class(const window_class&&) = delete;
    window_class& operator=(const window_class&&) = delete;

   private:
    window_class() noexcept;
    ~window_class();
    static constexpr auto name = "DXEngine Window";
    static window_class wc_;
    HINSTANCE h_instance_;
  };
};

#endif  // APP_H
