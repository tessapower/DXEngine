#include "DXengine.h"
#include "MessageMap.h"

#include <cassert>
#include <iostream>

static MessageMap messages;

LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void onResize(HWND hWnd, WPARAM wParam, const int width, const int height);

int CALLBACK wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  const auto pClassName = L"WindowMcWindowFace";

  // Create a new instance of an extended window class
  WNDCLASSEX wc = {
      sizeof(WNDCLASSEX),  // Size of structure in bytes
      CS_HREDRAW | CS_VREDRAW |
          CS_OWNDC,  // Class style(s), repaint on vertical & horizontal resize
      &wndProc,      // Pointer to window proc
      0,             // # extra bytes following window class structure
      0,             // # extra bytes following window instance
      hInstance,     // Handle to instance that contains window proc for class
      nullptr,       // Handle to class icon
      LoadCursor(hInstance,
                 IDC_ARROW),           // Handle to class cursor, standard arrow
      (HBRUSH)(COLOR_BACKGROUND + 1),  // Handle to class background brush
      nullptr,                         // Pointer to resource name of class menu
      pClassName,                      // Window class name
      nullptr                          // Handle to small icon
  };

  // Register the window class
  ATOM atom = RegisterClassEx(&wc);
  assert(atom != 0);

  // Create a rectangle to specify the client area dimension
  RECT client{0, 0, 800, 600};
  AdjustWindowRectEx(&client,                // Rect to use
                     WS_OVERLAPPEDWINDOW,    // Window style
                     false,                  // If window has a menu
                     WS_EX_OVERLAPPEDWINDOW  // Extended window style
  );

  // Create an extended window with all the bells and whistles
  HWND hWnd = CreateWindowEx(
      WS_EX_OVERLAPPEDWINDOW,  // Extended window style
      pClassName,              // Window class name
      L"Win Win Chick Din",    // Window name in title bar
      WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |
          WS_OVERLAPPEDWINDOW,     // Window style(s)
      CW_USEDEFAULT,               // x position of window
      CW_USEDEFAULT,               // y position of window
      client.right - client.left,  // Client width
      client.bottom - client.top,  // Client height
      nullptr,                     // Handle to parent window
      nullptr,                     // Handle to menu
      hInstance,  // Handle to instance of module to be associated with window
      nullptr     // Pointer to lpParam
  );

  // Do the thing!
  ShowWindow(hWnd, nCmdShow);

  // Listen for messages
  bool res;
  MSG msg;
  while ((res = GetMessage(&msg, nullptr, 0, 0)) > 0) {
    // Useful for when we want to process WM_CHAR messages!
    // We can always skip translating the message if we will
    // not bother processing WM_CHAR messages, e.g. if we are
    // only interested key down or up messages
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return EXIT_SUCCESS;
}

// Called every time we dispatch a message from the queue
LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  OutputDebugStringA(messages(uMsg, wParam, lParam).c_str());

  switch (uMsg) {
    case WM_CLOSE: {
      PostQuitMessage(0);
      break;
    }
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void onResize(HWND hWnd, WPARAM wParam, const int width, const int height) {}
