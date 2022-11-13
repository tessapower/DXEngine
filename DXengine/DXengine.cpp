#include "ChiliWin.h"
#include "Window.h"

#include <iostream> // EXIT_SUCCESS macro

int CALLBACK WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine,
                     _In_ int nCmdShow) {
  try {
    Window wnd;

    // Listen for messages
    bool res;
    MSG msg;
    while ((res = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      // Useful for when we want to process WM_CHAR messages!
      // We can always skip translating the message if we will
      // not bother processing WM_CHAR messages, e.g. if we are
      // only interested key down or up messages
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return msg.wParam;
  } catch (EngineException &e) {
    MessageBoxA(
      nullptr,
      e.what(),
      "Engine exception!",
      MB_ICONEXCLAMATION | MB_OK
    );
  } catch (std::exception &e) {
    MessageBoxA(
      nullptr,
      e.what(),
      "Exception!",
      MB_ICONEXCLAMATION | MB_OK
    );
  } catch (...) {
    MessageBoxA(
      nullptr,
      "An unknown error occurred!\nNo details available :(",
      "Wtf?",
      MB_ICONEXCLAMATION | MB_OK
    );
  }

  return EXIT_FAILURE;
}
