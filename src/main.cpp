#include "stdafx.h"

#include "app.h"

auto CALLBACK WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,
                      _In_ int) -> int {
  try {
    // Create a window
    app app(1280, 800, "DX Engine");

    // Initialize the GUI/Window
    app.init_gui();

    // Main loop
    bool done = false;
    while (!done) {
      app.tick(done);
    }
  } catch (const engine_exception& e) {
    MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    OutputDebugString(e.what());
    MessageBox(nullptr, e.what(), "std::exception", MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (...) {
    MessageBox(nullptr, "No details available", "Unknown Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
