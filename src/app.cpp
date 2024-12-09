#include "stdafx.h"
#include "app.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <cassert>
#include <sstream>

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

  // Initialize Direct3D
  if (!create_device_d3d(h_wnd_)) {
    cleanup_device_d3d();
    UnregisterClassW(reinterpret_cast<LPCWSTR>(window_class::class_name()),
                     window_class::h_instance());

    // TODO: throw some exception
  }
}

app::~app() {
  OutputDebugStringW(L"Initiating app shutdown sequence...\n");

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  cleanup_device_d3d();
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
      g_resize_width = static_cast<UINT>(LOWORD(l_param));  // Queue resize
      g_resize_height = static_cast<UINT>(HIWORD(l_param));

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

auto app::init_gui() const noexcept -> void {
  ShowWindow(h_wnd_, SW_SHOWDEFAULT);
  UpdateWindow(h_wnd_);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_Init(h_wnd_);  // Must be called after ImGui context created!
  ImGui_ImplDX11_Init(g_pd3d_device, g_pd3d_device_context);
}

auto app::set_title(const LPCWSTR title) const noexcept -> void {
  SetWindowTextW(h_wnd_, title);
}

auto app::update(bool &done) const noexcept -> void {
  // Poll and handle messages (inputs, window resize, etc.)
  // See the WndProc() function below for our to dispatch events to the Win32
  // backend.
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT) done = true;
  }
  if (done) return;

  // Handle window being minimized or screen locked
  if (g_swap_chain_occluded &&
      g_p_swap_chain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
    Sleep(10);
    return;
  }
  g_swap_chain_occluded = false;

  // Handle window resize (we don't resize directly in the WM_SIZE handler)
  if (g_resize_width != 0 && g_resize_height != 0) {
    cleanup_render_target();
    g_p_swap_chain->ResizeBuffers(0, g_resize_width, g_resize_height,
                                  DXGI_FORMAT_UNKNOWN, 0);
    g_resize_width = g_resize_height = 0;
    create_render_target();
  }

  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  {
    ImGui::Begin("Shaders");
    ImGui::Text(
        "This window will display a list of shaders that \nare dynamically "
        "loaded and applied to the scene.");

    ImGui::Spacing();

    ImGui::ColorEdit3(
        "clear color",
        const_cast<float *>(reinterpret_cast<const float *>(
            &clear_color_)));  // Edit 3 floats representing a color

    ImGui::Spacing();

    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
    //            1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
  }
}

auto app::render() const noexcept -> void {
  // Rendering
  ImGui::Render();
  const float clear_color_with_alpha[4] = {
      clear_color_.x * clear_color_.w, clear_color_.y * clear_color_.w,
      clear_color_.z * clear_color_.w, clear_color_.w};
  g_pd3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view,
                                            nullptr);
  g_pd3d_device_context->ClearRenderTargetView(g_main_render_target_view,
                                               clear_color_with_alpha);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // Present
  const HRESULT hr = g_p_swap_chain->Present(1, 0);  // Present with vsync
  // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
  g_swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
}

//-------------------------------------------------------------- Exception --//

auto app::exception::what() const noexcept -> const char * {
  return "App Exception";
}

auto app::exception::msg() const noexcept -> LPCWSTR {
  std::wostringstream oss;
  oss << "[Error Code] " << error_code() << "\n"
      << "[Description] " << error_string() << "\n"
      << source() << "\n";
  what_buffer_ = oss.str();

  return what_buffer_.c_str();
}

auto app::exception::error_code() const noexcept -> HRESULT { return hr_; }

auto app::exception::error_string() const noexcept -> std::wstring {
  return translate_error_code(hr_);
}

auto app::exception::translate_error_code(const HRESULT hr) noexcept
    -> std::wstring {
  wchar_t *msg_buf = nullptr;
  DWORD msg_len = FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPWSTR>(&msg_buf), 0, nullptr);

  if (msg_len == 0) return L"Unidentified error code";

  std::wstring error_str = msg_buf;
  LocalFree(msg_buf);

  return error_str;
}
