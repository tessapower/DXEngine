#include "app.h"

#include <cassert>
#include <sstream>

#include "message_map.h"
#include "resource.h"

static message_map messages;

//-------------------------------------------------------------------- App --//
app::app(const int width, const int height, const LPCWSTR window_title) {
  width_ = width;
  height_ = height;

  // Create a rectangle to specify the client area dimension
  RECT viewport{0, 0, width_, height_};
  HRESULT hr =
      AdjustWindowRectEx(&viewport,  // Rect to use
                                     // Window style(s)
                         WS_OVERLAPPED,
                         false,                  // If window has a menu
                         WS_EX_OVERLAPPEDWINDOW  // Extended window style(s)
      );

  // TODO: handle unsuccessful window creation

  // Create an extended window with all the bells and whistles
  h_wnd_ = CreateWindowExW(
      WS_EX_OVERLAPPEDWINDOW,                  // Extended window style(s)
      window_class::instance()->class_name(),  // Window class name
      window_title,                            // Window name in title bar
      WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,  // Window style
      CW_USEDEFAULT,                           // x position of window
      CW_USEDEFAULT,                           // y position of window
      viewport.right - viewport.left,          // Client width
      viewport.bottom - viewport.top,          // Client height
      nullptr,                                 // Handle to parent window
      nullptr,                                 // Handle to menu
      window_class::instance()->h_instance(),  // Handle to instance to
                                               // be associated with window
      this  // Pass a pointer to this instance of Window and
            // be able to access it from the created hWnd
  );

  // Do the thing!
  ShowWindow(h_wnd_, SW_SHOWDEFAULT);
}

app::~app() { DestroyWindow(h_wnd_); }

LRESULT CALLBACK app::handle_msg_setup(const HWND h_wnd, const UINT u_msg,
                                       const WPARAM w_param,
                                       const LPARAM l_param) noexcept {
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

LRESULT CALLBACK app::handle_msg_thunk(const HWND h_wnd, const UINT u_msg,
                                       const WPARAM w_param,
                                       const LPARAM l_param) noexcept {
  // Get a pointer to the Window associated with the given hWnd
  const auto p_window =
      reinterpret_cast<app *>(GetWindowLongPtrA(h_wnd, GWLP_USERDATA));

  // Forward on the message to the Window instance
  return p_window->handle_msg(h_wnd, u_msg, w_param, l_param);
}

// Called every time we dispatch a message from the queue
LRESULT app::handle_msg(const HWND h_wnd, const UINT u_msg,
                        const WPARAM w_param, const LPARAM l_param) noexcept {
  OutputDebugStringW(messages(u_msg, w_param, l_param).c_str());

  switch (u_msg) {
    case WM_CLOSE:
    case WM_QUIT: {
      PostQuitMessage(0);

      break;
    }
    case WM_KILLFOCUS: {
      break;
    }
    default: {
      return DefWindowProcW(h_wnd, u_msg, w_param, l_param);
    }
  }

  return 0;
}

//----------------------------------------------------------- Window Class --//
app::window_class *app::window_class::window_class_;

app::window_class::window_class() noexcept
    : h_instance_(GetModuleHandle(nullptr)) {
  // Create a new instance of an extended window class
  const WNDCLASSEXW win_class = {
      sizeof(WNDCLASSEX),  // Size of structure in bytes
      // Class styles, repaint on vertical & horizontal resize,
      // each window has its own device context
      CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      &handle_msg_setup,   // Pointer to initial setup window proc
      0,                   // # extra bytes following window class structure
      0,                   // # extra bytes following window instance
      h_instance_,         // Handle to instance with window proc for class
      static_cast<HICON>(  // Handle to window icon
          LoadImageA(h_instance_, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON,
                     // TODO: replace hardcoded icon sizes
                     32, 32, LR_DEFAULTCOLOR)),
      LoadCursor(h_instance_, IDC_ARROW),  // Handle to class cursor, standard
      reinterpret_cast<HBRUSH>(2),         // Handle to class background brush
      nullptr,             // Pointer to resource name of class menu
      name,                // Window class name
      static_cast<HICON>(  // Handle to small icon
          LoadImageA(h_instance_, MAKEINTRESOURCEA(IDI_ICON1), IMAGE_ICON, 16,
                     16, LR_DEFAULTCOLOR))};

  // Register WindowClass
  const ATOM atom = RegisterClassExW(&win_class);
  assert(atom != 0);
}

app::window_class::~window_class() {
  // Deregister WindowClass
  if (window_class_) {
    UnregisterClassW(class_name(), h_instance());
  }
}

app::window_class *app::window_class::instance() noexcept {
  if (!window_class_) {
    window_class_ = new window_class();
  }

  return window_class_;
}

HINSTANCE app::window_class::h_instance() noexcept {
  return window_class_->h_instance_;
}

LPCWSTR app::window_class::class_name() noexcept { return name; }

//-------------------------------------------------------------- Exception --//

auto app::exception::what() const noexcept -> const char * override {
  return "App Exception";
}

auto app::exception::msg() const noexcept -> LPCWSTR override {
  std::wostringstream oss;
  oss << "[Error Code] " << error_code << "\n"
      << "[Description] " << error_string << "\n"
      << source() << "\n";
  what_buffer_ = oss.str();

  return what_buffer_.c_str();
}

auto app::exception::error_code() const noexcept -> HRESULT { return hr_; }

auto app::exception::error_string() const noexcept -> std::wstring {
  return translate_error_code(hr_);
}

auto app::exception::translate_error_code(const HRESULT hr) noexcept -> std::wstring {
  wchar_t *msg_buf = nullptr;
  DWORD msg_len = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPWSTR>(&msg_buf), 0, nullptr);

  if (msg_len == 0) return L"Unidentified error code";

  std::wstring error_str = msg_buf;
  LocalFree(msg_buf);

  return error_str;
}
