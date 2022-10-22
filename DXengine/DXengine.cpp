#include "DXengine.h"

#include <cassert>
#include <iostream>

LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void onResize(HWND hWnd, WPARAM wParam, const int width, const int height);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  WNDCLASSEX wc = {
      sizeof(WNDCLASSEX),  // Size of structure in bytes
      CS_HREDRAW |
          CS_VREDRAW,  // Class styles, repaint on vertical & horizontal resize
      &wndProc,        // Pointer to window procedure
      0,               // # extra bytes following window-class structure
      0,               // # extra bytes following window instance
      hInstance,       // Handle to instance that contains window proc for class
      NULL,            // Handle to class icon
      LoadCursor(NULL, IDC_ARROW),     // Handle to class cursor, standard arrow
      (HBRUSH)(COLOR_BACKGROUND + 1),  // Handle to class background brush
      NULL,                            // Pointer to resource name of class menu
      L"MyWindow",                     // Window class name
      NULL                             // Handle to small icon
  };

  ATOM atom = RegisterClassEx(&wc);
  assert(atom != 0);

  // Create a rectangle to specify the client area dimension
  RECT client{0, 0, 800, 600};
  AdjustWindowRectEx(&client,                // Rect to use
                     WS_OVERLAPPEDWINDOW,    // Window style
                     false,                  // If window has a menu
                     WS_EX_OVERLAPPEDWINDOW  // Extended window style
  );

  HWND hWnd = CreateWindowExA(
      WS_EX_OVERLAPPEDWINDOW,      // Extended window style
      (LPCSTR)MAKEINTATOM(atom),   // Window class name
      "Win Win Chick Din",         // Window name
      WS_OVERLAPPED,               // Window style
      CW_USEDEFAULT,               // X position of window
      CW_USEDEFAULT,               // Y position of window
      client.right - client.left,  // Window width
      client.bottom - client.top,  // Window height
      NULL,                        // Handle to parent window
      NULL,                        // Handle to menu
      hInstance,  // Handle to instance of module to be associated with window
      NULL        // Pointer to lpParam
  );

  ShowWindow(hWnd, nCmdShow);

  return 0;
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
