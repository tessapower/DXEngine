#include "stdafx.h"

#include <iostream>

#include "app.h"

int CALLBACK wWinMain(_In_ HINSTANCE,
                      _In_opt_ HINSTANCE,
                      _In_ LPWSTR,
                      _In_ int) {
  try {
    app app;

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
    }

    return static_cast<int>(msg.wParam);
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
      L"Error!",
      MB_ICONEXCLAMATION | MB_OK
    );
  }

  return EXIT_FAILURE;
}
