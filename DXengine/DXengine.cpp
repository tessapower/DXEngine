#include "DXengine.h"

#include <cassert>
#include <iostream>

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
          CS_OWNDC,  // Class styles, repaint on vertical & horizontal resize
      &wndProc,      // Pointer to window procedure
      0,             // # extra bytes following window-class structure
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

  // Create an extended window
  HWND hWnd = CreateWindowEx(
      WS_EX_OVERLAPPEDWINDOW,  // Extended window style
      pClassName,              // Window class name
      L"Win Win Chick Din",    // Window name
      WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU |
          WS_OVERLAPPEDWINDOW,     // Window style
      CW_USEDEFAULT,               // X position of window
      CW_USEDEFAULT,               // Y position of window
      client.right - client.left,  // Window width
      client.bottom - client.top,  // Window height
      nullptr,                     // Handle to parent window
      nullptr,                     // Handle to menu
      hInstance,  // Handle to instance of module to be associated with window
      nullptr     // Pointer to lpParam
  );

  ShowWindow(hWnd, nCmdShow);

  // Listen for messages
  bool res;
  MSG msg;
  while ((res = GetMessage(&msg, nullptr, 0, 0)) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return EXIT_SUCCESS;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  OutputDebugStringA("Henlo\n");
  // switch (uMsg) {
  // case WM_SIZE: {
  //     int width = LOWORD(lParam);
  //     int height = HIWORD(lParam);
  //     onResize(hWnd, wParam, width, height);
  // }
  //}

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void onResize(HWND hWnd, WPARAM wParam, const int width, const int height) {}
