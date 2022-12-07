#include "ChiliWin.h"
#include "Game.h"
#include "Window.h"

#include <iostream> // EXIT_SUCCESS macro

int CALLBACK wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR lpCmdLine,
                     _In_ int nCmdShow) {
  try {
    Window wnd;
    Game game(wnd);

    // Listen for messages
    bool res;
    MSG msg;
    while ((res = GetMessage(&msg, nullptr, 0, 0)) != 0) {
      // Useful for when we want to process WM_CHAR messages!
      // We can always skip translating the message if we will
      // not bother processing WM_CHAR messages, e.g. if we are
      // only interested in key down or up messages
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      game.tick();
    }

    return msg.wParam;
  } catch (EngineException &e) {
    MessageBoxW(
      nullptr,
      e.msg(),
      e.type(),
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
    MessageBoxW(
      nullptr,
      L"An unknown error occurred!\nNo details available :(",
      L"wtf?",
      MB_ICONEXCLAMATION | MB_OK
    );
  }

  return EXIT_FAILURE;
}
