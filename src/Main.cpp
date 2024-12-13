#include "stdafx.h"
#include "app.h"
#include "imgui.h"

auto main(int, char**) -> int {
  try {
    // Create a window
    app app(1280, 800, L"DX Engine");

    // Initialize the GUI/Window
    app.init_gui();

    // Main loop
    bool done = false;
    while (!done) {
      app.update(done);
      app.render();
    }
  } catch (const engine_exception& e) {

    MessageBoxW(nullptr, e.msg(), e.type(), MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    OutputDebugStringA(e.what());
    MessageBoxA(nullptr, e.what(), "Standard Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (...) {
    MessageBoxW(nullptr, L"No details available", L"Unknown Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
