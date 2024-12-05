#include "stdafx.h"
#include "app.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

auto main(int, char**) -> int {
  // Create a window
  const app app(1280, 800, L"DX Engine");

  // Initialize the GUI/Window
  app.init_gui();

  // Main loop
  bool done = false;
  while (!done) {
    app.update(done);
    app.render();
  }

  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return EXIT_SUCCESS;
}
