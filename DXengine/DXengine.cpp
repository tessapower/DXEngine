#include "ChiliWin.h"
#include "Window.h"

#include <iostream> // EXIT_SUCCESS macro

int CALLBACK wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  Window wnd(800, 600, L"DXengine");

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

  // Return exit code from PostQuitMessage
  return EXIT_SUCCESS;
}
