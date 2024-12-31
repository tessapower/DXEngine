#include "stdafx.h"

#include "app.h"
#include "imgui.h"

auto CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int {
  try {
    // Create a window
    app app(1280, 800, "DX Engine");

    // Initialize the GUI/Window
    app.init_gui();

    // Main loop
    bool done = false;
    while (!done) {
      app.update(done);
      app.render();
    }
  } catch (const engine_exception& e) {

    MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    OutputDebugString(e.what());
    MessageBox(nullptr, e.what(), "Standard Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (...) {
    MessageBox(nullptr, "No details available", "Unknown Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
