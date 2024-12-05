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
    app.update(done);
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
