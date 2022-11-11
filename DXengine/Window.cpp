#include "Window.h"
#include "MessageMap.h"

#include <cassert>

static MessageMap messages;
Window::WindowClass *Window::WindowClass::_windowClass;

Window::WindowClass::WindowClass() noexcept
    : _hInstance(GetModuleHandle(nullptr)) {
  // Create a new instance of an extended window class
  WNDCLASSEX wc = {
      sizeof(WNDCLASSEX),  // Size of structure in bytes
      // Class style(s), repaint on vertical & horizontal resize
      CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      &handleMsgSetup,      // Pointer to initial setup window proc
      0,                    // # extra bytes following window class structure
      0,                    // # extra bytes following window instance
      hInstance,            // Handle to instance that contains window proc for class
      nullptr,              // Handle to class icon
      LoadCursor(hInstance, IDC_ARROW),  // Handle to class cursor, standard arrow
      (HBRUSH)(2),            // Handle to class background brush
      nullptr,                // Pointer to resource name of class menu
      name,                   // Window class name
      nullptr                 // Handle to small icon
  };

  // Register WindowClass
  ATOM atom = RegisterClassEx(&wc);
  assert(atom != 0);
}

Window::WindowClass::~WindowClass() {
  // Deregister WindowClass
  if (_windowClass) {
    UnregisterClass(_name, hInstance());
  }
}

Window::WindowClass *Window::WindowClass::instance() {
  if (!_windowClass) {
    _windowClass = new WindowClass();
  }

  return _windowClass;
}

HINSTANCE Window::WindowClass::hInstance() { return _windowClass->_hInstance; }

LPCWSTR Window::WindowClass::name() { return _name; }

Window::Window(int width, int height, LPCWSTR name) {
  _width = width;
  _height = height;

  // Create a rectangle to specify the client area dimension
  RECT client{0, 0, _width, _height};
  AdjustWindowRectEx(&client,                // Rect to use
                     // Window style(s)
                     WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                     false,                  // If window has a menu
                     WS_EX_OVERLAPPEDWINDOW  // Extended window style(s)
  );

  // Create an extended window with all the bells and whistles
  _hWnd = CreateWindowEx(
      WS_EX_OVERLAPPEDWINDOW,           // Extended window style(s)
      WindowClass::instance()->name(),  // Window class name
      name,                             // Window name in title bar
      WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX |
          WS_SYSMENU,              // Window style(s)
      CW_USEDEFAULT,               // x position of window
      CW_USEDEFAULT,               // y position of window
      client.right - client.left,  // Client width
      client.bottom - client.top,  // Client height
      nullptr,                     // Handle to parent window
      nullptr,                     // Handle to menu
      WindowClass::instance()
          ->hInstance(),  // Handle to instance of to be associated with window
      this  // Pass a pointer to this instance of Window and be able to access
            // it from the created hWnd
  );

  // Do the thing!
  ShowWindow(_hWnd, SW_SHOWDEFAULT);
}

Window::~Window() {
  DestroyWindow(_hWnd);
}

LRESULT CALLBACK Window::handleMsgSetup(HWND hWnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam) {
  OutputDebugStringA(messages(uMsg, wParam, lParam).c_str());

  if (uMsg == WM_NCCREATE) {
    // Retrieve the lpParam we passed in when creating the hWnd
    CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
    Window *pWindow = reinterpret_cast<Window *>(pCreate->lpCreateParams);

    // Set the USERDATA to point to this window
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);

    // Now set the WNDPROC to point to handleMsgThunk
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)handleMsgThunk);

    return pWindow->handleMsg(hWnd, uMsg, wParam, lParam);
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgThunk(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
  // Get a pointer to the Window associated with the given hWnd
  Window *pWindow = reinterpret_cast<Window *>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

  // Forward on the message to the Window instance
  return pWindow->handleMsg(hWnd, uMsg, wParam, lParam);
}

// Called every time we dispatch a message from the queue
LRESULT Window::handleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  OutputDebugStringA(messages(uMsg, wParam, lParam).c_str());

  switch (uMsg) {
    case WM_CLOSE: {
      PostQuitMessage(0);

      return 0;
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
