#include "MessageMap.h"
#include "Window.h"
#include "WindowExceptionMacros.h"
#include "resource.h"

#include <cassert>

static MessageMap messages;

//------------------------------------------------------------ WindowClass --//
Window::WindowClass *Window::WindowClass::_windowClass;

Window::WindowClass::WindowClass() noexcept
    : _hInstance(GetModuleHandle(nullptr)) {
  // Create a new instance of an extended window class
  WNDCLASSEXW wc = {
      sizeof(WNDCLASSEX),  // Size of structure in bytes
      // Class style(s), repaint on vertical & horizontal resize
      CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      &handleMsgSetup,      // Pointer to initial setup window proc
      0,                    // # extra bytes following window class structure
      0,                    // # extra bytes following window instance
      _hInstance,           // Handle to instance with window proc for class
      static_cast<HICON>(   // Handle to window icon
        LoadImageA(
          _hInstance,
          MAKEINTRESOURCEA(IDI_ICON1),
          IMAGE_ICON,
          32, 32,
          LR_DEFAULTCOLOR
        )
      ),
      LoadCursor(_hInstance, IDC_ARROW),  // Handle to class cursor, standard
      (HBRUSH)(2),            // Handle to class background brush
      nullptr,                // Pointer to resource name of class menu
      _name,                  // Window class name
      static_cast<HICON> (    // Handle to small icon
        LoadImageA(
          _hInstance,
          MAKEINTRESOURCEA(IDI_ICON1),
          IMAGE_ICON,
          16, 16,
          LR_DEFAULTCOLOR
        )
      )
  };

  // Register WindowClass
  ATOM atom = RegisterClassExW(&wc);
  assert(atom != 0);
}

Window::WindowClass::~WindowClass() {
  // Deregister WindowClass
  if (_windowClass) {
    UnregisterClassW(name(), hInstance());
  }
}

Window::WindowClass *Window::WindowClass::instance() noexcept {
  if (!_windowClass) {
    _windowClass = new WindowClass();
  }

  return _windowClass;
}

HINSTANCE Window::WindowClass::hInstance() noexcept {
  return _windowClass->_hInstance;
}

LPCWSTR Window::WindowClass::name() noexcept { return _name; }

//----------------------------------------------------------------- Window --//
Window::Window(int width, int height, LPCWSTR windowTitle) {
  _width = width;
  _height = height;

  // Create a rectangle to specify the client area dimension
  RECT client{0, 0, _width, _height};
  HRESULT hr = AdjustWindowRectEx(
      &client,  // Rect to use
      // Window style(s)
      WS_OVERLAPPED,
      false,                  // If window has a menu
      WS_EX_OVERLAPPEDWINDOW  // Extended window style(s)
  );

  if (!SUCCEEDED(hr)) {
    throw HR_ERROR(hr);
  }

  // Create an extended window with all the bells and whistles
  _hWnd = CreateWindowExW(
      WS_EX_OVERLAPPEDWINDOW,          // Extended window style(s)
      WindowClass::instance()->name(), // Window class name
      windowTitle,                     // Window name in title bar
      WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // Window style
      CW_USEDEFAULT,              // x position of window
      CW_USEDEFAULT,              // y position of window
      client.right - client.left, // Client width
      client.bottom - client.top, // Client height
      nullptr,                    // Handle to parent window
      nullptr,                    // Handle to menu
      WindowClass::instance()->hInstance(), // Handle to instance to
                                            // be associated with window
      this  // Pass a pointer to this instance of Window and
            // be able to access it from the created hWnd
  );

  if (_hWnd == nullptr) {
    throw LAST_ERROR;
  }

  // Do the thing!
  ShowWindow(_hWnd, SW_SHOWDEFAULT);
}

Window::~Window() {
  DestroyWindow(_hWnd);
}

LRESULT CALLBACK Window::handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) noexcept {
  OutputDebugStringW(messages(uMsg, wParam, lParam).c_str());

  if (uMsg == WM_NCCREATE) {
    // Retrieve the lpParam we passed in when creating the hWnd
    const CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
    Window *pWindow = reinterpret_cast<Window *>(pCreate->lpCreateParams);

    // Set the USERDATA to point to this window
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);

    // Now set the WNDPROC to point to handleMsgThunk
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)handleMsgThunk);

    return pWindow->handleMsg(hWnd, uMsg, wParam, lParam);
  }

  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) noexcept {
  // Get a pointer to the Window associated with the given hWnd
  Window *pWindow = reinterpret_cast<Window *>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

  // Forward on the message to the Window instance
  return pWindow->handleMsg(hWnd, uMsg, wParam, lParam);
}

// Called every time we dispatch a message from the queue
LRESULT Window::handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) noexcept {
  OutputDebugStringW(messages(uMsg, wParam, lParam).c_str());

  switch (uMsg) {
    case WM_CLOSE: {
      PostQuitMessage(0);
      break;
    }
    case WM_KILLFOCUS: {
      kbd.clearKeyStates();
      break;
    }
    case WM_KEYDOWN: case WM_SYSKEYDOWN: {
      unsigned char keyCode = (unsigned char)wParam;
      kbd.onKeyPress(keyCode);
      break;
    }
    case WM_KEYUP: case WM_SYSKEYUP: {
      unsigned char keyCode = (unsigned char)wParam;
      kbd.onKeyRelease(keyCode);
      break;
    }
    case WM_CHAR: {
      unsigned char keyCode = (unsigned char)wParam;
      kbd.onChar(keyCode);

      // TODO: Delete all below
      auto c = kbd.nextChar();
      if (c.has_value()) {
        wchar_t ch = c.value();
        static std::wostringstream stream;
        stream << ch;
        SetWindowTextW(_hWnd, stream.str().c_str());
      }

      break;
    }
    default: {
      return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
  }

  return 0;
}

//------------------------------------------------------- Window Exception --//
LPCWSTR Window::Exception::msg() const noexcept {
  std::wostringstream oss;
  oss << type()
      << std::endl
      << "HR Code: 0x" << std::hex << _hr
      << std::endl
      << "What: " << translateError(_hr)
      << std::endl
      << source();

  _whatBuffer = oss.str();
  
  return _whatBuffer.c_str();
}

std::wstring Window::Exception::translateError(HRESULT hr) noexcept {
  LPWSTR pMsg = nullptr;

  auto msgLen = FormatMessageW(
      FORMAT_MESSAGE_FROM_SYSTEM  // use system message tables to translate
          | FORMAT_MESSAGE_ALLOCATE_BUFFER  // allocate buffer on local heap
          | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,  // unused when FORMAT_MESSAGE_FROM_SYSTEM is present
      hr,       // the error code
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // language defaults
      (LPWSTR)&pMsg,  // where to put the error message
      0,             // minimum size for output buffer
      nullptr);      // unused arguments

  if (msgLen != 0) {
    std::wstring msg = pMsg;
    LocalFree(pMsg);
    pMsg = nullptr;

    return msg;
  }

  return L"Unknown error!";
}
