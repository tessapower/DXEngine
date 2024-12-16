#include "stdafx.h"
#include "app.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <cassert>
#include <sstream>

#include "exception_macros.h"
#include "gui.h"
#include "message_map.h"
#include "renderer.h"

namespace {
message_map messages;
}  // namespace
app::window_class app::window_class::wc_;

//----------------------------------------------------------- Window Class --//

app::window_class::window_class() noexcept
    : h_instance_(GetModuleHandle(nullptr)) {
  // Create application window

  // TODO: figure out if this can be done elsewhere?
  ImGui_ImplWin32_EnableDpiAwareness();

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_CLASSDC;
  wc.lpfnWndProc = &handle_msg_setup;
  wc.hInstance = h_instance();
  wc.lpszClassName = class_name();

  // Register WindowClass
  const ATOM atom = RegisterClassExW(&wc);
  assert(atom != 0);
}

app::window_class::~window_class() {
  // Deregister WindowClass
  UnregisterClassW(class_name(), h_instance());
}

auto app::window_class::h_instance() noexcept -> HINSTANCE {
  return wc_.h_instance_;
}

auto app::window_class::class_name() noexcept -> LPCWSTR { return name; }

//-------------------------------------------------------------------- App --//
app::app(const int width, const int height, const LPCWSTR window_title) {
  width_ = width;
  height_ = height;

  // Create a rectangle to specify the client area dimension
  RECT viewport{0, 0, width_, height_};
  [[maybe_unused]] HRESULT hr =
      AdjustWindowRectEx(&viewport,  // Rect to use
                                     // Window style(s)
                         WS_OVERLAPPED,
                         false,                  // If window has a menu
                         WS_EX_OVERLAPPEDWINDOW  // Extended window style(s)
      );

  // TODO: handle unsuccessful window creation

  // Create an extended window with all the bells and whistles
  h_wnd_ = CreateWindowExW(
      WS_EX_OVERLAPPEDWINDOW,      // Extended window style(s)
      window_class::class_name(),  // Window class name
      window_title,                // Window name in title bar
      WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,  // Window style
      CW_USEDEFAULT,                   // x position of window
      CW_USEDEFAULT,                   // y position of window
      viewport.right - viewport.left,  // Client width
      viewport.bottom - viewport.top,  // Client height
      nullptr,                         // Handle to parent window
      nullptr,                         // Handle to menu
      window_class::h_instance(),      // Handle to instance to be
                                       // associated with window
      this  // Pass a pointer to this instance of Window and
            // be able to access it from the created h_wnd
  );

  gui_.set_viewport(width, height);

  // Initialize Direct3D
  if (renderer_.create_device_d3d(h_wnd_) != S_OK) {
    renderer_.cleanup_device_d3d();
    UnregisterClassW(reinterpret_cast<LPCWSTR>(window_class::class_name()),
                     window_class::h_instance());

    // TODO: throw some exception

  }
}

app::~app() {
  OutputDebugStringW(L"Initiating app shutdown sequence...\n");

  renderer_.shut_down();

  DestroyWindow(h_wnd_);
  UnregisterClassW(window_class::class_name(), window_class::h_instance());
}

auto CALLBACK app::handle_msg_setup(const HWND h_wnd, const UINT u_msg,
                                    const WPARAM w_param,
                                    const LPARAM l_param) noexcept -> LRESULT {
  OutputDebugStringW(messages(u_msg, w_param, l_param).c_str());

  if (u_msg == WM_NCCREATE) {
    // Retrieve the lpParam we passed in when creating the hWnd
    const CREATESTRUCT *p_create = reinterpret_cast<CREATESTRUCT *>(l_param);
    auto p_window = static_cast<app *>(p_create->lpCreateParams);

    // Set the USERDATA to point to this window
    SetWindowLongPtr(h_wnd, GWLP_USERDATA,
                     reinterpret_cast<LONG_PTR>(p_window));

    // Now set the WNDPROC to point to handleMsgThunk
    SetWindowLongPtr(h_wnd, GWLP_WNDPROC,
                     reinterpret_cast<LONG_PTR>(&app::handle_msg_thunk));

    return p_window->handle_msg(h_wnd, u_msg, w_param, l_param);
  }

  return DefWindowProcW(h_wnd, u_msg, w_param, l_param);
}

auto CALLBACK app::handle_msg_thunk(const HWND h_wnd, const UINT u_msg,
                                    const WPARAM w_param,
                                    const LPARAM l_param) noexcept -> LRESULT {
  // Get a pointer to the window associated with the given h_wnd
  const auto p_window =
      reinterpret_cast<app *>(GetWindowLongPtrA(h_wnd, GWLP_USERDATA));

  // Forward on the message to the Window instance
  return p_window->handle_msg(h_wnd, u_msg, w_param, l_param);
}

// Called every time we dispatch a message from the queue
auto app::handle_msg(const HWND h_wnd, const UINT u_msg, const WPARAM w_param,
                     const LPARAM l_param) noexcept -> LRESULT {
  OutputDebugStringW(messages(u_msg, w_param, l_param).c_str());

  if (ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param))
    return true;

  switch (u_msg) {
    case WM_SIZE:
      if (w_param == SIZE_MINIMIZED) return 0;
      renderer_.resize_width =
          static_cast<UINT>(LOWORD(l_param));  // Queue resize
      renderer_.resize_height = static_cast<UINT>(HIWORD(l_param));

      return 0;
    case WM_DESTROY:
    case WM_CLOSE:
    case WM_QUIT:
      PostQuitMessage(0);

      return 0;
    case WM_KILLFOCUS:
      break;
    default:
      return DefWindowProcW(h_wnd, u_msg, w_param, l_param);
  }

  return 0;
}

auto app::init_gui() noexcept -> void {
  ShowWindow(h_wnd_, SW_SHOWDEFAULT);
  UpdateWindow(h_wnd_);

  // TODO: figure out actual viewport size
  gui_.init();
  gui_.set_viewport(width_, height_);

  // Setup Platform/Renderer backends
  renderer_.init_backends(h_wnd_);
}

auto app::set_title(const LPCWSTR title) const noexcept -> void {
  SetWindowTextW(h_wnd_, title);
}

auto app::update(bool &done) -> void {
  // Poll and handle messages (inputs, window resize, etc.)
  MSG msg;
  
  while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT) done = true;
  }
  if (done) return;

  // Handle window being minimized or screen locked
  if (renderer_.swap_chain_occluded &&
      renderer_.p_swap_chain->Present(0, DXGI_PRESENT_TEST) ==
          DXGI_STATUS_OCCLUDED) {
    Sleep(10);

    return;
  }
  renderer_.swap_chain_occluded = false;

  // TODO: use result of window resize
  // Handle window resize (we don't resize directly in the WM_SIZE handler)
  if (renderer_.resize_width != 0 && renderer_.resize_height != 0) {
    renderer_.cleanup_render_target();
    HRESULT hr;
    RNDR_THROW(renderer_.p_swap_chain->ResizeBuffers(
      0,
      renderer_.resize_width,
      renderer_.resize_height,
      DXGI_FORMAT_UNKNOWN, 0)
    );
    renderer_.resize_width = renderer_.resize_height = 0;
    renderer_.create_render_target();
  }

  // Start the Dear ImGui frame
  // TODO: put into renderer::new_frame() function
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();

  gui_.update();
}

auto app::render() noexcept -> void {
  gui_.render(renderer_);

  // TODO: put into renderer::present() function
  // Present
  const HRESULT hr =
      renderer_.p_swap_chain->Present(1, 0);  // Present with vsync
  // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
  renderer_.swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
}

//-------------------------------------------------------------- Exception --//

auto app::exception::what() const noexcept -> const char * {
  return "App Exception";
}

auto app::exception::msg() const noexcept -> LPCWSTR {
  std::wostringstream oss;
  oss << "[Error Code] 0x" << std::hex << std::uppercase << error_code()
      << std::dec << " (" << static_cast<unsigned long>(error_code()) << ")\n"
      << "[Description] " << error_string() << "\n"
      << source();

  what_buffer_ = oss.str();

  return what_buffer_.c_str();
}

auto app::exception::error_code() const noexcept -> HRESULT { return hr_; }

auto app::exception::error_string() const noexcept -> std::wstring {
  return translate_error_code(hr_);
}
