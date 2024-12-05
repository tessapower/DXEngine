#include "stdafx.h"
#include <d3d11.h>
#include <tchar.h>

#include "app.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "renderer.h"

auto main(int, char**) -> int {
  // Create a window
  const app app(1280, 800, L"DX Engine");

  // Initialize the GUI/Window
  app.init_gui();

  // Main loop
  bool done = false;
  while (!done) {
    // Poll and handle messages (inputs, window resize, etc.)
    // See the WndProc() function below for our to dispatch events to the Win32
    // backend.
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) done = true;
    }
    if (done) break;

    // Handle window being minimized or screen locked
    if (g_swap_chain_occluded &&
        g_p_swap_chain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
      Sleep(10);
      continue;
    }
    g_swap_chain_occluded = false;

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_resize_width != 0 && g_resize_height != 0) {
      cleanup_render_target();
      g_p_swap_chain->ResizeBuffers(0, g_resize_width, g_resize_height,
                                    DXGI_FORMAT_UNKNOWN, 0);
      g_resize_width = g_resize_height = 0;
      create_render_target();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
      ImGui::Begin("Shaders");
      ImGui::Text(
          "This window will display a list of shaders that \nare dynamically "
          "loaded and applied to the scene.");

      ImGui::Spacing();

      ImGui::ColorEdit3(
          "clear color",
          reinterpret_cast<float*>(
              &clear_color));  // Edit 3 floats representing a color

      ImGui::Spacing();

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w};
    g_pd3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view,
                                              nullptr);
    g_pd3d_device_context->ClearRenderTargetView(g_main_render_target_view,
                                                 clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    const HRESULT hr = g_p_swap_chain->Present(1, 0);  // Present with vsync
    // HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
    g_swap_chain_occluded = (hr == DXGI_STATUS_OCCLUDED);
  }

  // Cleanup
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return EXIT_SUCCESS;
}
