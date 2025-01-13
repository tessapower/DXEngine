#include "stdafx.h"

#include "app.h"

//#include <imgui_impl_dx11.h>
//#include <imgui_impl_win32.h>

#include <memory>
#include <cassert>
#include <sstream>

#include "exception_macros.h"
#include "gui.h"
#include "message_map.h"
#include "renderer/renderer.h"

namespace {
message_map messages;
}  // namespace
app::window_class app::window_class::wc_;

//----------------------------------------------------------- Window Class --//

app::window_class::window_class() noexcept
    : h_instance_(GetModuleHandle(nullptr)) {
  // Create application window

  // TODO: figure out if this can be done elsewhere?
  // TODO: uncomment to reintroduce IMGUI
  //ImGui_ImplWin32_EnableDpiAwareness();

  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(wc);
  wc.style = CS_CLASSDC;
  wc.lpfnWndProc = &handle_msg_setup;
  wc.hInstance = h_instance();
  wc.lpszClassName = class_name();

  // Register WindowClass
  const ATOM atom = RegisterClassEx(&wc);
  assert(atom != 0);
}

app::window_class::~window_class() {
  // Deregister WindowClass
  UnregisterClass(class_name(), h_instance());
}

auto app::window_class::h_instance() noexcept -> HINSTANCE {
  return wc_.h_instance_;
}

auto app::window_class::class_name() noexcept -> LPCSTR { return name; }

//-------------------------------------------------------------------- App --//
app::app(const int width, const int height, const LPCSTR window_title) {
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
  h_wnd_ = CreateWindowEx(
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

  p_gui_ = std::make_unique<gui>(width, height);

  // Initialize Direct3D
  // Create unique pointer to renderer
  p_renderer_ = std::make_unique<renderer>();

  if (FAILED(hr = p_renderer_->create_device_d3d(h_wnd_))) {
    UnregisterClass(window_class::class_name(), window_class::h_instance());
    
    throw HR_EXCEPT(hr);
  }
}

app::~app() {
  OutputDebugString("Initiating app shutdown sequence...\n");

  p_renderer_->shut_down();

  DestroyWindow(h_wnd_);
  UnregisterClass(window_class::class_name(), window_class::h_instance());
}

auto CALLBACK app::handle_msg_setup(const HWND h_wnd, const UINT u_msg,
                                    const WPARAM w_param,
                                    const LPARAM l_param) noexcept -> LRESULT {
  OutputDebugString(messages(u_msg, w_param, l_param).c_str());

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

  return DefWindowProc(h_wnd, u_msg, w_param, l_param);
}

// Invokes our member function by retrieving the pointer to
// window class and calling handle_msg on it.
auto CALLBACK app::handle_msg_thunk(const HWND h_wnd, const UINT u_msg,
                                    const WPARAM w_param,
                                    const LPARAM l_param) noexcept -> LRESULT {
  // Get a pointer to the window associated with the given h_wnd
  const auto p_window =
      reinterpret_cast<app *>(GetWindowLongPtr(h_wnd, GWLP_USERDATA));

  // Forward on the message to the Window instance
  return p_window->handle_msg(h_wnd, u_msg, w_param, l_param);
}

// Called every time we dispatch a message from the queue
auto app::handle_msg(const HWND h_wnd, const UINT u_msg, const WPARAM w_param,
                     const LPARAM l_param) noexcept -> LRESULT {
  OutputDebugString(messages(u_msg, w_param, l_param).c_str());

  if (ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param))
    return true;

  switch (u_msg) {
    case WM_SIZE:
      if (w_param == SIZE_MINIMIZED) return 0;
      // Queue resize
      p_renderer_->resize(static_cast<UINT>(LOWORD(l_param)),
                          static_cast<UINT>(HIWORD(l_param)));

      return 0;
    case WM_DESTROY:
    case WM_CLOSE:
    case WM_QUIT:
      PostQuitMessage(0);

      return 0;
    case WM_KILLFOCUS:
      break;
    default:
      return DefWindowProc(h_wnd, u_msg, w_param, l_param);
  }

  return 0;
}

auto app::init_gui() noexcept -> void {
  ShowWindow(h_wnd_, SW_SHOWDEFAULT);
  UpdateWindow(h_wnd_);

  // TODO: figure out actual viewport size

  // TODO: uncomment to reintroduce IMGUI
  //gui_.init();
  //gui_.set_viewport(width_, height_);

  // Setup Platform/Renderer backends
  //p_renderer_->init_backends(h_wnd_);
}

auto app::set_title(const LPCSTR title) const noexcept -> void {
  SetWindowText(h_wnd_, title);
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

  // Start the Dear ImGui frame
  // TODO: put into renderer::new_frame() function
  // TODO: uncomment to reintroduce IMGUI
  //ImGui_ImplDX11_NewFrame();
  //ImGui_ImplWin32_NewFrame();
  //gui_.update();
}

auto app::render() noexcept -> void {
  // TODO: uncomment to reintroduce IMGUI
  //gui_.render(renderer_);
  p_renderer_->clear_back_buffer(clear_color_);
  p_renderer_->test_draw();

  // Present
  p_renderer_->end_frame();
}
