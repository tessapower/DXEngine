#include "stdafx.h"

#include "app.h"
#include "imgui.h"

auto CALLBACK WinMain(_In_ HINSTANCE h_instance,
                      _In_opt_ HINSTANCE h_prev_instance,
                      _In_ LPSTR lp_cmd_line,
                      _In_ int n_cmd_show) -> int {
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
    MessageBox(nullptr, e.what(), "std::exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  } catch (...) {
    MessageBox(nullptr, "No details available", "Unknown Exception",
               MB_OK | MB_ICONEXCLAMATION);

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
